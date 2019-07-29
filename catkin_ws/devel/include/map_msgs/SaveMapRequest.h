// Generated by gencpp from file map_msgs/SaveMapRequest.msg
// DO NOT EDIT!


#ifndef MAP_MSGS_MESSAGE_SAVEMAPREQUEST_H
#define MAP_MSGS_MESSAGE_SAVEMAPREQUEST_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>

#include <std_msgs/String.h>

namespace map_msgs
{
template <class ContainerAllocator>
struct SaveMapRequest_
{
  typedef SaveMapRequest_<ContainerAllocator> Type;

  SaveMapRequest_()
    : filename()  {
    }
  SaveMapRequest_(const ContainerAllocator& _alloc)
    : filename(_alloc)  {
  (void)_alloc;
    }



   typedef  ::std_msgs::String_<ContainerAllocator>  _filename_type;
  _filename_type filename;





  typedef boost::shared_ptr< ::map_msgs::SaveMapRequest_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::map_msgs::SaveMapRequest_<ContainerAllocator> const> ConstPtr;

}; // struct SaveMapRequest_

typedef ::map_msgs::SaveMapRequest_<std::allocator<void> > SaveMapRequest;

typedef boost::shared_ptr< ::map_msgs::SaveMapRequest > SaveMapRequestPtr;
typedef boost::shared_ptr< ::map_msgs::SaveMapRequest const> SaveMapRequestConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::map_msgs::SaveMapRequest_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::map_msgs::SaveMapRequest_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace map_msgs

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': False, 'IsMessage': True, 'HasHeader': False}
// {'nav_msgs': ['/opt/ros/kinetic/share/nav_msgs/cmake/../msg'], 'std_msgs': ['/opt/ros/kinetic/share/std_msgs/cmake/../msg'], 'actionlib_msgs': ['/opt/ros/kinetic/share/actionlib_msgs/cmake/../msg'], 'sensor_msgs': ['/opt/ros/kinetic/share/sensor_msgs/cmake/../msg'], 'geometry_msgs': ['/opt/ros/kinetic/share/geometry_msgs/cmake/../msg'], 'map_msgs': ['/home/lei/catkin_ws/src/navigation_msgs/map_msgs/msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::map_msgs::SaveMapRequest_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::map_msgs::SaveMapRequest_<ContainerAllocator> const>
  : FalseType
  { };

template <class ContainerAllocator>
struct IsMessage< ::map_msgs::SaveMapRequest_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::map_msgs::SaveMapRequest_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::map_msgs::SaveMapRequest_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::map_msgs::SaveMapRequest_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::map_msgs::SaveMapRequest_<ContainerAllocator> >
{
  static const char* value()
  {
    return "716e25f9d9dc76ceba197f93cbf05dc7";
  }

  static const char* value(const ::map_msgs::SaveMapRequest_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x716e25f9d9dc76ceULL;
  static const uint64_t static_value2 = 0xba197f93cbf05dc7ULL;
};

template<class ContainerAllocator>
struct DataType< ::map_msgs::SaveMapRequest_<ContainerAllocator> >
{
  static const char* value()
  {
    return "map_msgs/SaveMapRequest";
  }

  static const char* value(const ::map_msgs::SaveMapRequest_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::map_msgs::SaveMapRequest_<ContainerAllocator> >
{
  static const char* value()
  {
    return "\n\
std_msgs/String filename\n\
\n\
\n\
================================================================================\n\
MSG: std_msgs/String\n\
string data\n\
";
  }

  static const char* value(const ::map_msgs::SaveMapRequest_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::map_msgs::SaveMapRequest_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.filename);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct SaveMapRequest_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::map_msgs::SaveMapRequest_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::map_msgs::SaveMapRequest_<ContainerAllocator>& v)
  {
    s << indent << "filename: ";
    s << std::endl;
    Printer< ::std_msgs::String_<ContainerAllocator> >::stream(s, indent + "  ", v.filename);
  }
};

} // namespace message_operations
} // namespace ros

#endif // MAP_MSGS_MESSAGE_SAVEMAPREQUEST_H