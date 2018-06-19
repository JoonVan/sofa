#include "AnimationLoopTasks.h"

#include <sofa/core/behavior/BaseAnimationLoop.h>
#include <sofa/core/ExecParams.h>
#include <sofa/core/ConstraintParams.h>
#include <sofa/core/MechanicalParams.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/helper/AdvancedTimer.h>
//#include <sofa/helper/system/atomic.h>

namespace sofa
{
    
    namespace simulation
    {
        
        
        
        StepTask::StepTask(core::behavior::BaseAnimationLoop* aloop, const double t, Task::Status* pStatus)
        : Task(pStatus)
        , animationloop(aloop)
        , dt(t)
        {
        }
        
        StepTask::~StepTask()
        {
        }
        
        
        bool StepTask::run(WorkerThread* )
        {
            animationloop->step( core::ExecParams::defaultInstance(), dt);
            return true;
        }
        
        
        
        
        //InitPerThreadDataTask::InitPerThreadDataTask(volatile long* atomicCounter, boost::mutex* mutex, TaskStatus* pStatus )
        InitPerThreadDataTask::InitPerThreadDataTask(std::atomic<int>* atomicCounter, std::mutex* mutex, Task::Status* pStatus )
        : Task(pStatus), IdFactorygetIDMutex(mutex), _atomicCounter(atomicCounter)
        {}
        
        InitPerThreadDataTask::~InitPerThreadDataTask()
        {
        }
        
        bool InitPerThreadDataTask::run(WorkerThread* )
        {
            
            core::ExecParams::defaultInstance();
            
            core::ConstraintParams::defaultInstance();
            
            core::MechanicalParams::defaultInstance();
            
            core::visual::VisualParams::defaultInstance();
            
            // init curTimerThread
            //helper::getCurTimer();
            //std::stack<AdvancedTimer::IdTimer>& getCurTimer();
            {
                // to solve IdFactory<Base>::getID() problem in AdvancedTimer functions
                std::lock_guard<std::mutex> lock(*IdFactorygetIDMutex);
                
                //spinMutexLock lock( IdFactorygetIDMutex );
                
                helper::AdvancedTimer::begin("Animate");
                helper::AdvancedTimer::end("Animate");
                //helper::AdvancedTimer::stepBegin("AnimationStep");
                //helper::AdvancedTimer::stepEnd("AnimationStep");
            }
            
            
            //BOOST_INTERLOCKED_DECREMENT( mAtomicCounter );
            //BOOST_COMPILER_FENCE;
            
            _atomicCounter->fetch_sub(1, std::memory_order_acq_rel);
            
            
            while(_atomicCounter->load(std::memory_order_relaxed) > 0)
            {
                // yield while waiting
                std::this_thread::yield();
            }
            return false;
        }
        
        
    } // namespace simulation
    
} // namespace sofa




