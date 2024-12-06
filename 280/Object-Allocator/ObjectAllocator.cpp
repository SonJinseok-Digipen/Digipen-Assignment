#include "ObjectAllocator.h"
#include <iostream>
#include <cstring>
#include <cstdint>
ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig &config) : Config_(config)
{
    Stats_.ObjectSize_ = ObjectSize;
    Stats_.PageSize_ = Calculate_Page_Size();

    // Make Big Size Buffer
    char *buffer = new char[Stats_.PageSize_];
    std::memset(buffer, 0xAA, Stats_.PageSize_);
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
        // std::memset(FreeList_,0xAA,Stats_.ObjectSize_);
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
        Stats_.MostObjects_++;
        std::memset(target, 0xBB, Stats_.ObjectSize_);
        return target;
    }
    else
    {
        if (Stats_.PagesInUse_ < Config_.MaxPages_)
        {
            char *buffer = new char[Stats_.PageSize_];
            GenericObject *NewPageList_ = reinterpret_cast<GenericObject *>(buffer);
            NewPageList_->Next = PageList_;
            PageList_ = NewPageList_;

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
            Stats_.MostObjects_++;
            std::memset(target, 0xBB, Stats_.ObjectSize_);
            return target;
        }

        else
        {
            throw OAException(OAException::E_NO_PAGES, "");
        }
    }

    return nullptr;
}

void ObjectAllocator::Free([[maybe_unused]] void *Object)
{
    unsigned char *ptr = reinterpret_cast<unsigned char *>(Object);
    bool isAlreadyCleared = true;

    // 객체의 모든 바이트가 0xCC인지 확인
    for (size_t i = 0; i < Stats_.ObjectSize_; ++i)
    {
        if (ptr[i] != 0xCC)
        {
            isAlreadyCleared = false;
            break;
        }
    }

    if (isAlreadyCleared)
    {
        // 이미 0xCC로 설정된 경우, 에러 발생
        throw OAException(OAException::E_BAD_BOUNDARY, "Memory is already set to 0xCC");
    }

    bool onBound = false;
    GenericObject *page = PageList_;

    // 페이지 내에서 객체 주소가 올바른 경계에 있는지 확인
    while (page != nullptr)
    {
        unsigned char *pageStart = reinterpret_cast<unsigned char *>(page);
       [[maybe_unused]] unsigned char *pageEnd = pageStart + Stats_.PageSize_;

        // 페이지 내에서 객체가 올바른 범위에 있는지 확인
        if (reinterpret_cast<unsigned char *>(page) < ptr && ptr < reinterpret_cast<u_char *>(page) + Stats_.PageSize_)
        {
            unsigned char *blockStart = pageStart + sizeof(void *);
                                 
            size_t separation = Stats_.ObjectSize_;
                                

            // 객체가 블록 경계에 위치하는지 체크
            if (0 == (ptr - blockStart) % separation)
            {
                onBound = true;
                break;
            }
        }
        page = page->Next;
    }

    // 만약 객체가 경계에 맞지 않으면 예외 발생
    if (!onBound)
    {
        std::cout << "Error: Object not on a block boundary" << std::endl;
        throw OAException(OAException::E_BAD_BOUNDARY, "Object is not aligned to a block boundary");
    }
     // FreeList_가 nullptr인 경우 새로운 프리 리스트에 객체 추가
    if (FreeList_ == nullptr)
    {
        FreeList_ = reinterpret_cast<GenericObject *>(Object);
        FreeList_->Next = nullptr;
        Stats_.Deallocations_++;
        Stats_.ObjectsInUse_--;
        Stats_.FreeObjects_++;
        std::memset(FreeList_, 0xCC, Stats_.ObjectSize_);
    }
    else
    {
        GenericObject *NewFreeList_ = reinterpret_cast<GenericObject *>(Object);
        NewFreeList_->Next = FreeList_;
        std::memset(NewFreeList_, 0xCC, Stats_.ObjectSize_);
        FreeList_ = NewFreeList_;
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
