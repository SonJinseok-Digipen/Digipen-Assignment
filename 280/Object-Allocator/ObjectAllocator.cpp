#include"ObjectAllocator.h"
#include<iostream>
ObjectAllocator::ObjectAllocator(size_t ObjectSize,const OAConfig&config):Config_(config)
{
  Stats_.ObjectSize_=ObjectSize;
  Stats_.PageSize_=Calculate_Page_Size();
  PageList_=  reinterpret_cast<GenericObject*>(new char[Stats_.PageSize_]);
  FreeList_=  reinterpret_cast<GenericObject*>(reinterpret_cast<char*>(PageList_)+sizeof(GenericObject));
  
  

}

ObjectAllocator::~ObjectAllocator()
{

}

void* ObjectAllocator::Allocate(const char*label)
{
   return nullptr;
}

void ObjectAllocator::Free(void*Object)
{

}

unsigned ObjectAllocator::DumpMemoryInUse(ObjectAllocator::DUMPCALLBACK fn) const
{
   return 0;
}

unsigned ObjectAllocator::ValidatePages(ObjectAllocator::VALIDATECALLBACK fn) const
{
   return 0;
}

unsigned ObjectAllocator::FreeEmptyPages()
{
  return 0;
}
void ObjectAllocator::SetDebugState(bool State)
{

}

const void* ObjectAllocator::GetFreeList() const
{
    return FreeList_;
}

const void* ObjectAllocator::GetPageList() const
{
    return PageList_;
}

OAConfig ObjectAllocator::GetConfig() const
{
    return Config_;
}

OAStats ObjectAllocator::GetStats() const
{
    return Stats_;
}

size_t ObjectAllocator::Calculate_Page_Size()
{
    size_t total_size = sizeof(GenericObject*)+ (Config_.ObjectsPerPage_*Stats_.ObjectSize_);
    return total_size;
}

