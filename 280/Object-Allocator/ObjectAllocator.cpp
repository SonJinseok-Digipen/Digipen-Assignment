#include "ObjectAllocator.h"
#include <iostream>
ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig &config) : Config_(config)
{
    Stats_.ObjectSize_ = ObjectSize;
    Stats_.PageSize_ = Calculate_Page_Size();

    char *buffer = new char[Stats_.PageSize_];
    PageList_ = reinterpret_cast<GenericObject *>(buffer);
    FreeList_ = reinterpret_cast<GenericObject *>(reinterpret_cast<char *>(PageList_) + sizeof(GenericObject));
    std::cout << "PageList_ " << PageList_ << '\n';
    for (size_t i = 0; i < Config_.ObjectsPerPage_ - 1; i++)
    {
        GenericObject *NewFreeList_ = reinterpret_cast<GenericObject *>(reinterpret_cast<char *>(FreeList_) + Stats_.ObjectSize_);
        NewFreeList_->Next = FreeList_;
        FreeList_ = NewFreeList_;
    }
    Stats_.FreeObjects_ += Config_.ObjectsPerPage_;
    Stats_.PagesInUse_++;
}

ObjectAllocator::~ObjectAllocator()
{

    while (PageList_)
    {
        GenericObject *temp = PageList_->Next;
        delete[] reinterpret_cast<char *>(PageList_);
        PageList_ = temp;
    }
}

void *ObjectAllocator::Allocate([[maybe_unused]] const char *label)
{
    // If We have extra free space
    void *target = reinterpret_cast<void *>(FreeList_);
    if (target != nullptr)
    {
        FreeList_ = FreeList_->Next;
        Stats_.FreeObjects_--;
        return target;
    }

    // If we don't have free space it mean our FreeList_ point nullptr
    else if (Stats_.PagesInUse_ < Config_.MaxPages_)
    {
        // std::cout << "Create New Page " <<Stats_.PageSize_    <<'\n';
        char *buffer = new char[Stats_.PageSize_];
        GenericObject *NewPageList = reinterpret_cast<GenericObject *>(buffer);
        NewPageList->Next = PageList_;
        PageList_ = NewPageList;
        FreeList_ = reinterpret_cast<GenericObject *>(reinterpret_cast<char *>(PageList_) + sizeof(GenericObject));
        Stats_.PagesInUse_++;
        for (size_t i = 0; i < Config_.ObjectsPerPage_ - 1; i++)
        {
            GenericObject *NewFreeList_ = reinterpret_cast<GenericObject *>(reinterpret_cast<char *>(FreeList_) + Stats_.ObjectSize_);
            NewFreeList_->Next = FreeList_;
            FreeList_ = NewFreeList_;
        }
        void *target = reinterpret_cast<void *>(FreeList_);
        FreeList_ = FreeList_->Next;
        return target;
    }
    return nullptr;
}

void ObjectAllocator::Free([[maybe_unused]]void *Object)
{
}

unsigned ObjectAllocator::DumpMemoryInUse([[maybe_unused]]ObjectAllocator::DUMPCALLBACK fn) const
{
    return 0;
}

unsigned ObjectAllocator::ValidatePages([[maybe_unused]]ObjectAllocator::VALIDATECALLBACK fn) const
{
    return 0;
}

unsigned ObjectAllocator::FreeEmptyPages()
{
    return 0;
}
void ObjectAllocator::SetDebugState([[maybe_unused]]bool State)
{
}

const void *ObjectAllocator::GetFreeList() const
{
    return FreeList_;
}

const void *ObjectAllocator::GetPageList() const
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
    size_t total_size = sizeof(GenericObject *) + (Config_.ObjectsPerPage_ * Stats_.ObjectSize_);
    return total_size;
}
