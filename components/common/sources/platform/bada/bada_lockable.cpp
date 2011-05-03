#include <FBaseRtThreadMutex.h>
#include <platform/platform.h>
#include <xtl/common_exceptions.h>
using namespace common;


void BadaPlatform::InitLockable (lockable_t& lockable) {
    try {

        // ������ ������� ��� bad'�
        Osp::Base::Runtime::Mutex* mutex = new Osp::Base::Runtime::Mutex();

        // ���� ������� �� ��������, �� ����������
        if(mutex->Create() != E_SUCCESS) {
            throw xtl::make_null_argument_exception("", "lockable");
        }

        // ��������� �������
        lockable = mutex;

    } catch (xtl::exception& exception) {
        exception.touch("common::BadaPlatform::InitLockable");
        throw;  
    }
}


void BadaPlatform::DestroyLockable (lockable_t& lockable) {

    try {

        // ���� �������� ���, �� ����������
        if (!lockable) {
            throw xtl::make_null_argument_exception("", "lockable");
        }

        // �������� ��������
        Osp::Base::Runtime::Mutex* mutex = (Osp::Base::Runtime::Mutex*)lockable;
        mutex->~Mutex();
        delete reinterpret_cast<Osp::Base::Runtime::Mutex*> (lockable);
        lockable = 0;

    } catch (xtl::exception& exception) {
        exception.touch("common::BadaPlatform::DestroyLockable");
        throw;
    }
}


void BadaPlatform::Lock (lockable_t& lockable) {
    try {
    
        // ���� �������� ���, �� ����������
        if (!lockable) {
            throw xtl::make_null_argument_exception("", "lockable");
        }

        Osp::Base::Runtime::Mutex* mutex = (Osp::Base::Runtime::Mutex*)lockable;

        // ���� ������������� �� �������, �� ����������
        if (mutex->Acquire() != E_SUCCESS) {
            throw xtl::format_operation_exception("::Acquire", "Operation failed. Reason: Mutex is already acquired by another thread");
        }    

    } catch (xtl::exception& exception) {
        exception.touch("common::BadaPlatform::Lock");
        throw;
    }
}


void BadaPlatform::Unlock (lockable_t& lockable) {
    try {
    
        // ���� �������� ���, �� ����������
        if (!lockable) {
            throw xtl::make_null_argument_exception("", "lockable");
        }

        Osp::Base::Runtime::Mutex* mutex = (Osp::Base::Runtime::Mutex*)lockable;

        // ���� ���������� �� �������, �� ����������
        if (mutex->Release() != E_SUCCESS) {
            throw xtl::format_operation_exception("::Release", "Operation failed. Reason: Unknown");
        }
    
    } catch (xtl::exception& exception) {
        exception.touch ("common::BadaPlatform::Unlock");
        throw;
    }

}
