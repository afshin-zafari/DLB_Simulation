#include "dtsw.hpp"
//#include "util.hpp"
#define LOG_DTSW_DATA 0
namespace dtsw{
  /*----------------------------------------*/
  Parameters_t Parameters;
  Data *F1,*F2,*F3,*F4;
  Data *H1,*H2,*H3,*H4;
  Data *H,*T,*D;
  SWAlgorithm *sw_engine;
  /*----------------------------------------------------*/
  void runStep(SWTask*);
  IterationData *TimeStepsTask::D = nullptr;
  int TimeStepsTask::last_step=0;
  /*----------------------------------------*/
  /*----------------------------------------*/
  void parse_args(int argc,char *argv[]){
    Parameters.partition_level[0].M              = config.M;
    Parameters.partition_level[0].N              = config.N;
    Parameters.partition_level[0].blocks_per_row = 1;
    Parameters.partition_level[0].blocks_per_col = 1;
    
    Parameters.partition_level[1].M              = config.M / config.Mb;
    Parameters.partition_level[1].N              = config.N / config.Nb;
    Parameters.partition_level[1].blocks_per_row = config.Mb;
    Parameters.partition_level[1].blocks_per_col = config.Nb;

    Parameters.partition_level[2].M              = config.M / config.Mb / config.mb;
    Parameters.partition_level[2].N              = config.N / config.Nb / config.nb;
    Parameters.partition_level[2].blocks_per_row = config.mb;
    Parameters.partition_level[2].blocks_per_col = config.nb;

    Parameters.P = config.P;
    Parameters.p = config.p;
    Parameters.q = config.q;
    
    
  }
  /*----------------------------------------*/
  /*----------------------------------------*/
  void init(int argc, char *argv[]){

  }
  /*----------------------------------------*/
  void finalize(){
    sw_engine->finalize();
    delete sw_engine;
  }
  /*----------------------------------------------------*/
  void run(int argc, char *argv[]){
    TimeStepsTask::D = new IterationData();
    int n = (Parameters.IterNo<10)?Parameters.IterNo:10;
    for(int i=0; i < n; i++){
      TimeStepsTask *step = new TimeStepsTask;
      sw_engine->submit(step);
    }
    sw_engine->flush();
  }
  /*----------------------------------------------------*/
  void TimeStepsTask::finished(){
    SWTask::finished();
    
    LOG_INFO(LOG_DTSW,"step :%d, Par.StepNo :%d\n",last_step, Parameters.IterNo);
    if ( last_step  < Parameters.IterNo ) {        
      sw_engine->submit(new TimeStepsTask );
      sw_engine->flush();
    }
    LOG_INFO(LOG_DTSW,"(****)TimeStepTask::fin H(0) version is  write:%s read:%s .\n",
	     (*H)(0).getWriteVersion().dumpString().c_str(),
	     (*H)(0).getReadVersion().dumpString().c_str() );
    LOG_INFO(LOG_DTSW,"(****)TimeStePDATA gt-ver: rd %s, wr %s --- rt-ver: rd %s wr %s\n",
	     TimeStepsTask::D->getReadVersion().dumpString().c_str(),
	     TimeStepsTask::D->getWriteVersion().dumpString().c_str() ,
	     TimeStepsTask::D->getRunTimeVersion(IData::READ).dumpString().c_str(),
	     TimeStepsTask::D->getRunTimeVersion(IData::WRITE).dumpString().c_str());
  }
  /*----------------------------------------------------*/
  void TimeStepsTask::runKernel(){
#if ATOMIC_COUNTER
    is_submitting = true;
    runStep(this);
    is_submitting = false;
    sw_engine->flush();
#else
    runStep(this);
#endif
    LOG_INFO(LOG_DTSW,"DT Tasks count:%d.\n",sw_engine->get_tasks_count());
    LOG_INFO(LOG_DTSW,"(****)TimeStepTask::fin H(0) version is  write:%s read:%s .\n",
	     (*H)(0).getWriteVersion().dumpString().c_str(),
	     (*H)(0).getReadVersion().dumpString().c_str() );
    LOG_INFO(LOG_DTSW,"(****)TimeStePDATA gt-ver: rd %s, wr %s --- rt-ver: rd %s wr %s\n",
	     TimeStepsTask::D->getReadVersion().dumpString().c_str(),
	     TimeStepsTask::D->getWriteVersion().dumpString().c_str() ,
	     TimeStepsTask::D->getRunTimeVersion(IData::READ).dumpString().c_str(),
	     TimeStepsTask::D->getRunTimeVersion(IData::WRITE).dumpString().c_str());
    
    if (sw_engine->get_tasks_count()<=2)// Only time step tasks are added.
      finished();
    
  }
  /*----------------------------------------------------*/
  void runStep(SWTask *p){
    return;
  }
  /*----------------------------------------------------------------*/
  TimeStepsTask::~TimeStepsTask(){
    LOG_INFO(LOG_DTSW,"step :%d, Par.StepNo :%d\n",last_step, Parameters.IterNo);
    if ( last_step  < Parameters.IterNo )         
      sw_engine->submit(new TimeStepsTask );
  }
  /*----------------------------------------------------------------*/
  void TimeStepsTask::register_data(){
    parent_context = sw_engine;
    setName("TStep");
    TimeStepsTask::D->setName("TimeStepsData");
    if ( last_step ==1)
      TimeStepsTask::D->setRunTimeVersion("0.0",0);      
    IDuctteipTask::key = key;
    DataAccessList *dlist = new DataAccessList;    
    //    data_access(dlist,TimeStepsTask::D,(last_step==1)?IData::READ:IData::WRITE);
    setDataAccessList(dlist);
    LOG_INFO(LOG_DTSW,"(****)TimeStePDATA gt-ver: rd %s, wr %s --- rt-ver: rd %s wr %s\n",
	     TimeStepsTask::D->getReadVersion().dumpString().c_str(),
	     TimeStepsTask::D->getWriteVersion().dumpString().c_str() ,
	     TimeStepsTask::D->getRunTimeVersion(IData::READ).dumpString().c_str(),
	     TimeStepsTask::D->getRunTimeVersion(IData::WRITE).dumpString().c_str());
    LOG_INFO(LOG_DTSW,"(****) H(0) version is  write:%s read:%s .\n",
	     (*H)(0).getWriteVersion().dumpString().c_str(),
	     (*H)(0).getReadVersion().dumpString().c_str() );
    host = me;
  }
  /*----------------------------------------------------------------*/
  DTSWData::DTSWData(){      
    memory_type = USER_ALLOCATED;
    host_type=SINGLE_HOST;
    IData::parent_data = NULL;
    setDataHandle( sw_engine->createDataHandle(this));
    setDataHostPolicy( glbCtx.getDataHostPolicy() ) ;
    setLocalNumBlocks(1,1);
    IData::Mb = 0;
    IData::Nb = 0;
    setHostType(SINGLE_HOST);
    setParent(sw_engine);
    sw_engine->addInputData(this);
    LOG_INFO(LOG_DTSW_DATA,"Data handle for new dtswdata:%d\n",my_data_handle->data_handle);
    setRunTimeVersion("0.0",0);
  }
  /*----------------------------------------------------------------*/
  IterationData::IterationData(){      
    memory_type = USER_ALLOCATED;
    host_type=ALL_HOST;
    IData::parent_data = NULL;
    setDataHandle( sw_engine->createDataHandle(this));
    setDataHostPolicy( glbCtx.getDataHostPolicy() ) ;
    setLocalNumBlocks(1,1);
    IData::Mb = 0;
    IData::Nb = 0;
    setHostType(ALL_HOST);
    setParent(sw_engine);
    sw_engine->addInputData(this);
    setRunTimeVersion("0.0",0);
  }
  /*----------------------------------------------------------------*/
  void SGSWData::partition_data(DTSWData &d,int R,int C){
  }
  /*----------------------------------------------------------------------------*/
  DTSWData::DTSWData (int M, int N, int r,int c, std::string n,int total_size_in_bytes, int item_size_, bool isSparse)
    {
      nnz=0;
      setName(n);
      item_size = item_size_;
    for(int j=0;j<c;j++){
      for(int i=0;i<r;i++){
	DTSWData*t=new DTSWData;
	t->row_idx = i;
	t->col_idx = j;
	t->sp_row  = i;
	t->sp_col  = j;
	std::stringstream ss;
	if ( c>1)
	  ss << n << "(" << i << "," << j << ")";
	else
	  ss << n << "(" << i <<  ")";
	t->name.assign(ss.str());
	t->item_size = item_size_;
	t->memory_p = nullptr;
	if(!isSparse){
	  int partition_size = total_size_in_bytes / r/ c ;
	  t->mem_size_in_bytes = partition_size;
	  t->memory_p = new byte[partition_size];
	  t->mem_size_in_elements = partition_size / item_size_;
	  LOG_INFO(LOG_DTSW_DATA,"Memory :%p L2 mem  size(in bytes):%d (in items count):%d.\n",t->memory_p,t->mem_size_in_bytes, t->mem_size_in_elements);
	  int B = Parameters.partition_level[1].Nb;
	  t->setHost(i / ( B / Parameters.P ));	    
	  LOG_INFO(LOG_DTSW,"Host for %s is set to %d /  %d / %d   = %d .\n",ss.str().c_str(),i,B,Parameters.P,i / ( B / Parameters.P ));
	  t->setHostType(SINGLE_HOST);
	  t->allocateMemory();
	}
	else{// for sparse data D 
	  t->setHostType(ALL_HOST);
	  t->setHost(-1);	    
	}
	t->sg_data =nullptr;
	Dlist.push_back(t);
      }
    }
    rows = r;
    cols = c;
    row_idx=col_idx = -1;
    sg_data = nullptr;
    memory_p = nullptr;
  }
  /*---------------------------------------------
  RHSTask(Data &a, Data &b, Data &c,SWTask *p){
    A = static_cast<Data *>(&a);
    B = static_cast<Data *>(&b);
    C = static_cast<Data *>(&c);
    child_count = 0;
    parent = p;
    if (p)
      step_no = p->step_no;
    host = C->getHost();
    atm_offset = a.get_block_row() * Parameters.atm_block_size_L1;
    key = RHS;
    setNameWithParent("_RHS");	  
    *this << *A << *B >> *C;
    if(getHost() == me ) 
      if(parent)
	Atomic::increase(&parent->child_count);
  }
  ---------------------------------------------*/
  void DTSWData::report_data(){
    LOG_INFO(LOG_DTSW_DATA,"Data %s at (%d,%d) has memory at address:%p size(in-bytes):%d, size in elems:%d\n",
	     name.c_str(),row_idx,col_idx,memory_p, mem_size_in_bytes, mem_size_in_elements );
    for(auto d:Dlist){
      d->report_data();
    }
    if (sg_data)
      sg_data->report_data();
  }
  /*---------------------------------------------*/
  void SGSWData ::report_data(){
  }
  /*---------------------------------------------*/
  void SWAlgorithm::local_finished(){
    if ( !Parameters.pure_mpi ){
      stringstream fn;
      fn << "execution_" << Parameters.P << "_" << Parameters.partition_level[0].M <<"_B"
	 <<Parameters.partition_level[1].blocks_per_row << "_b"
	 <<Parameters.partition_level[2].blocks_per_row << "_"
	 << me << ".log";
      Trace<Options>::dump(fn.str().c_str());
    }
  }
}

