#include "ObjectAllocator.h"
#include <iostream>
ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig &config) : Config_(config)
{
    Stats_.ObjectSize_ = ObjectSize;
    Stats_.PageSize_ = Calculate_Page_Size();

    // Make Big Size Buffer
    char *buffer = new char[Stats_.PageSize_];

    // Reinterpret cast first 4 byte GenericObject
    PageList_ = reinterpret_cast<GenericObject *>(buffer);
    PageList_->Next = nullptr;

    // Reinterpret cast first 4 byte after PageList
    FreeList_ = reinterpret_cast<GenericObject *>(buffer + sizeof(GenericObject));
    FreeList_->Next = nullptr;

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
    if (FreeList_ != nullptr)
    {
        void *target = FreeList_;
        FreeList_ = FreeList_->Next;
        Stats_.Allocations_++;
        Stats_.ObjectsInUse_++;
        Stats_.FreeObjects_--;

        return target;
    }
    else
    {
        if (Stats_.PagesInUse_ < Config_.MaxPages_)
        {
            char *buffer = new char[Stats_.PageSize_];
            GenericObject *NewPageList_ = reinterpret_cast<GenericObject *>(buffer);
            NewPageList_->Next = PageList_;
            PageList_=NewPageList_;

            FreeList_ = reinterpret_cast<GenericObject *>(buffer + sizeof(GenericObject));
            FreeList_->Next = nullptr;
            for (size_t i = 0; i < Config_.ObjectsPerPage_ - 1; i++)
            {
                GenericObject *NewFreeList_ = reinterpret_cast<GenericObject *>(reinterpret_cast<char *>(FreeList_) + Stats_.ObjectSize_);
                NewFreeList_->Next = FreeList_;
                FreeList_ = NewFreeList_;
            }
            Stats_.FreeObjects_ += Config_.ObjectsPerPage_;
            Stats_.PagesInUse_++;

            void *target = FreeList_;
            FreeList_ = FreeList_->Next;

            Stats_.Allocations_++;
            Stats_.ObjectsInUse_++;
            Stats_.FreeObjects_--;

            return target;
        }

        else
        {
          throw OAException(OAException::E_NO_PAGES,"");
        }
    }

    return nullptr;
}

void ObjectAllocator::Free([[maybe_unused]] void *Object)
{
   if(FreeList_==nullptr)
   {
     FreeList_=reinterpret_cast<GenericObject*>(Object);
     FreeList_->Next=nullptr;
     Stats_.Deallocations_++;
     Stats_.ObjectsInUse_--;
     Stats_.FreeObjects_++;

   }
   else
   {
        GenericObject*NewFreeList_=reinterpret_cast<GenericObject*>(Object);
        NewFreeList_->Next=FreeList_;
        FreeList_=NewFreeList_;
         Stats_.FreeObjects_++;
        Stats_.Deallocations_++;
        Stats_.ObjectsInUse_--;
   }

}

unsigned ObjectAllocator::DumpMemoryInUse([[maybe_unused]] ObjectAllocator::DUMPCALLBACK fn) const
{
    return 0;
}

unsigned ObjectAllocator::ValidatePages([[maybe_unused]] ObjectAllocator::VALIDATECALLBACK fn) const
{
    return 0;
}

unsigned ObjectAllocator::FreeEmptyPages()
{
    return 0;
}
void ObjectAllocator::SetDebugState([[maybe_unused]] bool State)
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
