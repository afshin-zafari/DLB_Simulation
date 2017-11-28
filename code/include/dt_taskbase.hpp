#ifndef DT_TASKBASE_HPP
#define DT_TASKBASE_HPP

#include <list>
#include <atomic>
#include "dt_database.hpp"
#include "ductteip.hpp"
#include "sg/platform/atomic.hpp"

namespace dtsw{
  class SWTask: public IDuctteipTask {
  private:
  public:
    bool  is_submitting, sub_tasks_submitted;
    SWTask *parent;
    SGHandle sg_handle_child,sg_handle_comm;
    //std::atomic<size_t> child_count;
    int child_count;
    int step_no;
    virtual void dump()=0;
    virtual void runKernel()=0;
    virtual ~SWTask(){}
    /*------------------------------------------------------------*/
    SWTask &operator <<(Data &d1){ // Read data
      DataAccess *daxs = new DataAccess;
      IData *d = &d1;
      daxs->data = d;
      daxs->required_version = d->getWriteVersion();
      daxs->required_version.setContext( glbCtx.getLevelString() );
      d->getWriteVersion().setContext( glbCtx.getLevelString() );
      d->getReadVersion() .setContext( glbCtx.getLevelString() );
      daxs->type = IData::READ;
      data_list->push_back(daxs);
      d->incrementVersion(IData::READ);
      LOG_INFO(LOG_DTSW,"(****)Daxs Read %s for  %s is %p\n",
	       getName().c_str(), d1.getName().c_str(),daxs);
      return *this;
	
    }
    /*------------------------------------------------------------*/
    SWTask &operator >>(Data &d1){// Write Data 
      DataAccess *daxs = new DataAccess;
      IData *d = &d1;
      daxs->data = d;
      daxs->required_version = d->getReadVersion();
      daxs->required_version.setContext( glbCtx.getLevelString() );
      d->getWriteVersion().setContext( glbCtx.getLevelString() );
      d->getReadVersion() .setContext( glbCtx.getLevelString() );
      daxs->type = IData::WRITE;
      data_list->push_back(daxs);
      d->incrementVersion(IData::WRITE);
      LOG_INFO(LOG_DTSW,"(****)Daxs Write %s for  %s is %p\n",
	       getName().c_str(),d1.getName().c_str(),daxs);
      return *this;
    }
    /*------------------------------------------------------------*/
    SWTask(){
      data_list = new list<DataAccess*>;
      LOG_INFO(LOG_DTSW,"(****)Daxs dlist new %p\n",data_list);
      child_count = 0;
      parent = nullptr;
      is_submitting = false;
      sub_tasks_submitted = false;
    }
    /*------------------------------------------------------------*/
    void after_check_dependencies(){
      //      submit_next_level_tasks();
    }
    /*------------------------------------------------------------*/
    virtual void finished(){
      LOG_INFO(LOG_DTSW,"\n");
      if ( state >= Finished  ) return;
      setFinished(true);
      LOG_INFO(LOG_DTSW,"\n");
      if ( parent){
	if (  parent->child_count ==0 )
	  return;
	LOG_INFO(LOG_DTSW,"\n");
	if ( Atomic::decrease_nv(&parent->child_count) ==0)
	  {
	    LOG_INFO(LOG_DTSW,"child task :%s is waiting for parent: %s finishes submission.\n",getName().c_str(),parent->getName().c_str());
	    LOG_INFO(LOG_DTSW, "%s finished from parent's task :%s child_count :%d\n " ,getName().c_str(),parent->getName().c_str(),(int)parent->child_count );
	    parent->finished();
	}
      }
      LOG_INFO(LOG_DTSW,"\n");    
    }
    /*------------------------------------------------------------*/
    virtual void submit_next_level_tasks(){}
    /*------------------------------------------------------------*/
    bool is_still_submitting(){return is_submitting;}
    /*------------------------------------------------------------*/
    void set_submitting(bool f);
    /*------------------------------------------------------------*/
    void setNameWithParent(const char*n){
      if ( !parent ){
	setName(n);
	return;
      }
      std::stringstream ss;
      ss << parent->getName() << "_" << parent->child_count << n ;
      setName(ss.str());
    }

  };
}
#endif //DT_TASKBASE_HPP
