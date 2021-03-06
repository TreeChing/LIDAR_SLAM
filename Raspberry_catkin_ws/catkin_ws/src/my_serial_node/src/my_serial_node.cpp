/*********************************
 * 串口节点，订阅cmd_vel话题并发布odometry话题
 * 从cmd_vel话题中分解出速度值通过串口送到移动底盘
 * 从底盘串口接收里程消息整合到odometry话题用于发布
 * 
 * https://github.com/lei01cao/LIDAR_SLAM/tree/master/catkin_ws/src/my_serial_node
 * *******************************/
#include <ros/ros.h>
#include <serial/serial.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <tf/transform_broadcaster.h>

#define	sBUFFERSIZE	6//send buffer size 串口发送缓存长度
#define	rBUFFERSIZE	9//receive buffer size 串口接收缓存长度
const double ROBOT_LENGTH = 0.15025;
static unsigned char s_buffer[sBUFFERSIZE];//发送缓存
static unsigned char r_buffer[rBUFFERSIZE];//接收缓存
double x_ , vx_;
double y_ , vy_;
double th_, vth_;
ros::Time current_time, last_time;
boost::array<double, 36> odom_pose_covariance = {
    {1e-9, 0, 0, 0, 0, 0, 
    0, 1e-3, 1e-9, 0, 0, 0, 
    0, 0, 1e6, 0, 0, 0,
    0, 0, 0, 1e6, 0, 0, 
    0, 0, 0, 0, 1e6, 0, 
    0, 0, 0, 0, 0, 1e-9}};
boost::array<double, 36> odom_twist_covariance = {
    {1e-9, 0, 0, 0, 0, 0, 
    0, 1e-3, 1e-9, 0, 0, 0, 
    0, 0, 1e6, 0, 0, 0, 
    0, 0, 0, 1e6, 0, 0, 
    0, 0, 0, 0, 1e6, 0, 
    0, 0, 0, 0, 0, 1e-9}};
/************************************
 * 串口数据发送格式共6字节
 * head head   v_left_hex dir_left_hex  v_right_hex  dir_right_hex 
 * 0xff 0xff   u8         u8            u8           u8
 * **********************************/
/**********************************************************
 * 串口接收数据格式共9字节
 * v_left  v_right CRC
 * float   float   u8
 * ********************************************************/

//联合体，用于浮点数与16进制的快速转换
typedef union{
	unsigned char cvalue[4];
	float fvalue;
}float_union;

union odometry
{
	float odoemtry_float;
	unsigned char odometry_char[4];
}vel_left, vel_right;

serial::Serial ser;

/**********************************************************
 * 数据打包，将获取的cmd_vel信息打包并通过串口发送
 * ********************************************************/
void data_pack(const geometry_msgs::Twist& cmd_vel){
	//unsigned char i;
	float v_left,v_right;
	unsigned char v_left_hex,v_right_hex,dir_left_hex,dir_right_hex;
	float r,yawrate;
	float_union Vx,Vy,Ang_v;
	Vx.fvalue = cmd_vel.linear.x;
	Ang_v.fvalue = cmd_vel.angular.z;

    yawrate = Ang_v.fvalue;
	r = Vx.fvalue / yawrate;
	if(Vx.fvalue == 0)
	{
	   v_left = -yawrate * 0.09;
	   v_right = yawrate * 0.09;
	}
	else if(yawrate == 0)
	{
	   v_left = Vx.fvalue;
	   v_right = Vx.fvalue;		
	}
	else
	{
	  v_left = yawrate * (r - 0.09);
	  v_right = yawrate * (r + 0.09);
	}
	if(v_left > 0)
		dir_left_hex = 0;
	else
		dir_left_hex = 1;

	if(v_right > 0)
		dir_right_hex = 1;
	else
		dir_right_hex = 0;

	v_left_hex = (fabs)((int)(v_left*0.571*0.96/ 0.0064)); //0.96为计算误差补偿系数
	v_right_hex = (fabs)((int)(v_right*0.571*0.96 / 0.0064));    //0.96为计算误差补偿系数
	memset(s_buffer,0,sizeof(s_buffer));

	/********head********/
	s_buffer[0] = 0xff;
	s_buffer[1] = 0xfe;

	/****MOTOR SPEED*****/
	s_buffer[2] = v_left_hex;    //Left_Motor_Speed
	s_buffer[3] = dir_left_hex;  //Left_Motor_Dir
	s_buffer[4] = v_right_hex;   //Right_Motor_Speed
	s_buffer[5] = dir_right_hex; //Right_Motor_Dir

	// //数据打包
	
	ser.write(s_buffer,sBUFFERSIZE);
	memset(s_buffer,0,sizeof(s_buffer));
}
unsigned char getCrc8(unsigned char *ptr, unsigned short len)
{
	unsigned char crc;
	unsigned char i;
	crc = 0;
	while(len--)
	{
		crc ^= *ptr++;
		for(i = 0; i < 8; i++)
		{
			if(crc&0x01)
                crc=(crc>>1)^0x8C;
			else 
                crc >>= 1;
		}
	}
	return crc;
}

bool readSpeed()
{
	int i;
	unsigned char checkSum;
    //ROS_INFO("read odom.");
    // 读取串口数据
	ser.read(r_buffer,rBUFFERSIZE);
	ros::Time curr_time;

	checkSum = getCrc8(r_buffer, 8);

    if (checkSum != r_buffer[8])
	{
		ROS_ERROR("Received data check sum error!");
        memset(r_buffer,0,rBUFFERSIZE);
        return false;
	}
	ROS_INFO("Received data check Sum sucess!");

    for(i = 0; i < 4; i++)
    {
        vel_left.odometry_char[i]  = r_buffer[i];
        vel_right.odometry_char[i] = r_buffer[i + 4];
    }
    //ROS_INFO("v_left = %f", vel_left.odoemtry_float);
    // ROS_INFO("v_right = %f", vel_right.odoemtry_float);

    // 积分计算里程计信息
    vx_  = (vel_right.odoemtry_float + vel_left.odoemtry_float) / 2;
    vth_ = (vel_right.odoemtry_float - vel_left.odoemtry_float) / ROBOT_LENGTH;
    
    curr_time = ros::Time::now();

    double dt = (curr_time - last_time).toSec();
    double delta_x = (vx_ * cos(th_) - vy_ * sin(th_)) * dt;
    double delta_y = (vx_ * sin(th_) + vy_ * cos(th_)) * dt;
    double delta_th = vth_ * dt;

    x_ += delta_x;
    y_ += delta_y;
    th_ += delta_th;
    last_time = curr_time;   

    memset(r_buffer,0,rBUFFERSIZE);
	return true;
}
//接收数据分析与校验
unsigned char data_analysis(unsigned char *buffer)
{
	unsigned char ret=0,csum;
	int i;
	if((buffer[0]==0xaa) && (buffer[1]==0xaa)){
		csum = buffer[2]^buffer[3]^buffer[4]^buffer[5]^buffer[6]^buffer[7]^
				buffer[8]^buffer[9]^buffer[10]^buffer[11]^buffer[12]^buffer[13]^
				buffer[14]^buffer[15]^buffer[16]^buffer[17]^buffer[18]^buffer[19]^
				buffer[20]^buffer[21]^buffer[22]^buffer[23]^buffer[24]^buffer[25];
		ROS_INFO("check sum:0x%02x",csum);
		if(csum == buffer[26]){
			ROS_INFO("check success!");
			ret = 1;//校验通过，数据包正确
		}
		else 
		  ret =0;//校验失败，丢弃数据包
	}
	
	// for(i=0;i<rBUFFERSIZE;i++)
	  // ROS_INFO("0x%02x",buffer[i]);
	
	return ret;

}

//订阅turtle1/cmd_vel话题的回调函数，用于显示速度以及角速度
void cmd_vel_callback(const geometry_msgs::Twist& cmd_vel){
	 //ROS_INFO("I heard linear velocity: x-[%f],y-[%f],",cmd_vel.linear.x,cmd_vel.linear.y);
	 //ROS_INFO("I heard angular velocity: [%f]",cmd_vel.angular.z);
	// std::cout << "Twist Received" << std::endl;	
	data_pack(cmd_vel);
}
int main (int argc, char** argv)
{
    ros::init(argc, argv, "my_serial_node");
    ros::NodeHandle nh;

	//订阅/turtle1/cmd_vel话题用于测试 $ rosrun turtlesim turtle_teleop_key
	ros::Subscriber write_sub = nh.subscribe("/cmd_vel",1000,cmd_vel_callback);
	//发布里程计话题 odom
	ros::Publisher read_pub = nh.advertise<nav_msgs::Odometry>("odom",1000);

    try
    {
        ser.setPort("/dev/ttyUSB0");
        ser.setBaudrate(9600);
        serial::Timeout to = serial::Timeout::simpleTimeout(100);
        ser.setTimeout(to);
        ser.open();
    }
    catch (serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port ");
        return -1;
    }

    if(ser.isOpen()){
        ROS_INFO_STREAM("Serial Port initialized");
    }else{
        return -1;
    }
	//定义tf 对象
	static tf::TransformBroadcaster odom_broadcaster_;
	current_time = ros::Time::now();
	last_time = ros::Time::now();

    //10hz频率执行
    ros::Rate loop_rate(10);
    while(ros::ok())
    {
        ros::spinOnce();

        if(ser.available())
        {

            ROS_INFO_STREAM("Reading from serial port");
            readSpeed();
			//ROS_INFO_STREAM("End reading from serial port");

            current_time = ros::Time::now();

            // publish TF
            geometry_msgs::TransformStamped odom_trans;
		    odom_trans.header.stamp = current_time;
		    odom_trans.header.frame_id = "odom";
		    odom_trans.child_frame_id  = "base_footprint";

		    geometry_msgs::Quaternion odom_quat;
		    odom_quat = tf::createQuaternionMsgFromYaw(th_);
		    odom_trans.transform.translation.x = x_;
		    odom_trans.transform.translation.y = y_;
		    odom_trans.transform.translation.z = 0.0;
		    odom_trans.transform.rotation = odom_quat;
		    // ROS_INFO("rotation = %f",odom_quat.w);
		    odom_broadcaster_.sendTransform(odom_trans);

		    // 发布里程计消息
		    nav_msgs::Odometry msgl;
		    msgl.header.stamp = current_time;
		    msgl.header.frame_id = "odom";

		    msgl.pose.pose.position.x = x_;
		    msgl.pose.pose.position.y = y_;
		    msgl.pose.pose.position.z = 0.0;
		    msgl.pose.pose.orientation = odom_quat;
		    msgl.pose.covariance = odom_pose_covariance;
		    ROS_INFO("x_ = %f",x_);
		    ROS_INFO("y_ = %f",y_);
            ROS_INFO("th_ = %f", th_);
            ROS_INFO("orientation = %f",odom_quat.w);
		    msgl.child_frame_id = "base_footprint";
		    msgl.twist.twist.linear.x = vx_;
		    msgl.twist.twist.linear.y = vy_;
		    msgl.twist.twist.angular.z = vth_;
            ROS_INFO("vx_ = %f",vx_);
            ROS_INFO("vy_ = %f",vy_);
            ROS_INFO("vth_ = %f",vth_);

		    msgl.twist.covariance = odom_twist_covariance;
		    read_pub.publish(msgl);
        }
        loop_rate.sleep();

    }
}

