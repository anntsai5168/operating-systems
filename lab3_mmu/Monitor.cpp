#include "Monitor.h"
 
 
///////////////////////////////// access global variables & functions ////////////////////////////////
 
extern std::vector<frame_t>* frame_table;
extern std::deque<frame_t*>* free_list;
extern std::vector<Process>* proc_list;
extern Process* cur_proc;
extern Pager* pgr;
 
// flags
extern bool O;
extern bool P;
extern bool F;
extern bool S;
 
////////////////////////////////////////// define functions /////////////////////////////////////////
 
frame_t *Monitor::allocate_frame_from_free_list() {
 
   if (free_list->size() != 0) {
       frame_t* pf = free_list->front();
       free_list->pop_front();
       return pf;
   }
   return nullptr;       
}
 
 
frame_t *Monitor::get_frame(){
   frame_t* frame = allocate_frame_from_free_list();
   if (frame == nullptr) {
       frame = NULL;
       frame = pgr->select_victim_frame();
   }
   return frame;
}
 
 
void Monitor::process_exit(Process * cur_proc){
  
   if (O) {std::cout << "EXIT current process " << cur_proc->pid << std::endl;}
 
   for (int i = 0; i < cur_proc->PT.size(); i++) {
       if (cur_proc->PT[i].VALID){
           // invalid
           cur_proc->PT[i].VALID = 0;
           // frame table update
           frame_table->at(cur_proc->PT[i].frame_index).PT_pointer = nullptr;
           // return to free pool
           free_list->push_back(&frame_table->at(cur_proc->PT[i].frame_index));
 
           // unmap msg
           cur_proc->statistics.unmaps++;
           if (O) {std::cout << " UNMAP " << cur_proc->pid << ":" << i << std::endl;}
           // FOUT modified filemapped pages.
           if (cur_proc->PT[i].MODIFIED && cur_proc->PT[i].isFilemapped) {
               cur_proc->statistics.fouts++;
               if (O) {std::cout << " FOUT" << std::endl;}
           }
       }
       cur_proc->PT[i].PAGEDOUT = 0;          // cuz we handled this time, so reset for future changing.
   }
}
 
 
void Monitor::set_up_pte(Process * cur_proc, PTE* pte) {
   for (int i= 0; i < cur_proc->vma_list.size(); i ++){
       VMA* cur_vma = &cur_proc->vma_list[i];
       if ( pte->vaddr >= cur_vma->start_vpage && pte->vaddr <= cur_vma->end_vpage ) {
           pte->isVMA = 1;
           pte->WRITE_PROTECT = (unsigned int) cur_vma->write_protected;
           pte->isFilemapped = (unsigned int) cur_vma->file_mapped;
           break;
       }
   }
}
 
 
void Monitor::update_r_and_m_bits(Process * cur_proc, char instr, PTE* pte) {
   if (instr == 'r'){
       pte->REFERENCED = 1;
   }
   else {                                   // write
       if (pte->WRITE_PROTECT) {
           cur_proc->statistics.segprot ++;
           if (O) {std::cout << " SEGPROT" << std::endl;}
       }
       else{
           pte->MODIFIED = 1;
       }
       pte->REFERENCED = 1;                 // reference bit(s) are set whenever a page is used
   }
}
 
 
void Monitor::map(Process * cur_proc, frame_t * frame, PTE* pte, Pager* pgr) {
   // update frame table entry
   frame->pid = cur_proc->pid;
   frame->PT_pointer = pte;
 
   pte = &(cur_proc->PT[vpage_num]);
  
   // update page table entry
   pte->VALID = 1;
   pte->frame_index = (unsigned int) frame->fid;
   pte->REFERENCED = 0;
   pte->MODIFIED = 0;

   // update age
   frame->age = 0;
   frame->last_referenced = inst_count;
 
   // PRINT SUMMARY & COST
   if (pte->isFilemapped) {                  // “FIN” in case it is a memory mapped file
       if (O) {std::cout << " FIN" << std::endl;}
          
       cur_proc->statistics.fins++;
   }
   else if (pte->PAGEDOUT){                  // brought back from the swap space (“IN”)
       if (O) {std::cout << " IN" << std::endl;}
       cur_proc->statistics.ins ++;
   }
   else {                                    // was never swapped out and is not file mapped
       if (O) {std::cout << " ZERO" << std::endl;}
       cur_proc->statistics.zeros ++;
   }
   if (O) {std::cout << " MAP " << frame->fid << std::endl;}
   cur_proc->statistics.maps++;
}
 
 
void Monitor::unmap(frame_t * frame) {

   if (frame->PT_pointer){
       if (O) {std::cout << " UNMAP " << frame->pid << ":" << frame->PT_pointer->vaddr << std::endl;}
 
       // unmap not necessary to be cur_proc, this should depend on frame->pid
       proc_list->at(frame->pid).statistics.unmaps++;

       //std::cout << " UNMAP " << frame->pid << ":" << frame->PT_pointer->vaddr << std::endl;
       //std::cout << " UNMAP frame_index "  << frame->fid  << std::endl;
 
       frame->PT_pointer->VALID = 0;
  
       if (frame->PT_pointer->MODIFIED){
           if(frame->PT_pointer->isFilemapped){
               proc_list->at(frame->pid).statistics.fouts++;
               if (O) {std::cout << " FOUT" << std::endl;}  
           }
           else {
               frame->PT_pointer->PAGEDOUT = 1;
               proc_list->at(frame->pid).statistics.outs++;
               if (O) {std::cout << " OUT" << std::endl;}
           }
       }
   }
   // no need to update frame table entry, do this in map()
}
 
 
/////////////////////////////////////////// initialization ///////////////////////////////////////////
 
 
Monitor::Monitor(algorithm pgr_type, std::string *file, int size, std::string *rfile) :
   proc_list_obj()
 
// body
{  
  
   pgr = PagerInterface::createPager(pgr_type, rfile, &inst_count);
 
  
   // point to process list
   proc_list = &proc_list_obj;
 
   // parse input file   
   std::ifstream inputFile;
   std::string cur_line;
 
   inputFile.open(*file);
      
   while (getline(inputFile, cur_line) && !inputFile.eof()){
          
           // ignore comments
           if (cur_line[0] != '#') {
              
               auto line_length = strlen(cur_line.c_str());
 
               // proc_num & vmas
               if (line_length < 3){
                  
                   std::istringstream stream(cur_line);
                  
                   if (proc_num == 0) {
                       stream >> proc_num;
                   }
                   else {
                       stream >> vmas;
                      
                       // assign process value
                       Process new_process = Process(vmas, pid);   // cur pid
                      
                       // add to process list
                       proc_list->push_back(new_process);
                       pid ++;
 
                       // build page table
                       if (pid == proc_num) {                      // proc_list completely built
                           for (int i = 0; i < proc_list->size(); i ++){
                               for (int j = 0; j < PT_size; j ++){
                                   proc_list->at(i).PT[j].vaddr = (unsigned int) j;    // force tranfer, or couldnt assign value
                               }
                           }                           
                       }
                   }
               }
 
               // process info
               if (line_length > 4) {  
 
                   std::istringstream stream(cur_line);
                   stream >> start_vpage_in >> end_vpage_in >> write_protected_in >> file_mapped_in;
 
                   // create new VMA object
                   VMA vma_group = {
                       .start_vpage = start_vpage_in,
                       .end_vpage = end_vpage_in,
                       .write_protected = write_protected_in,
                       .file_mapped = file_mapped_in
                   };
 
                   // push in to VMA list
                   proc_list->at(pid - 1).vma_list.push_back(vma_group);     // cuz we do pid ++ after creating new process
 
               }
              
               // instructions
               if (line_length == 3 | line_length == 4){
                  
                   inst_count ++;
                  
                   std::istringstream stream(cur_line);
                   stream >> instr >> vpage_num;
 
                   // print out instr
                   if (O) {printf("%lu: ==> %c %d\n", inst_count-1, instr, vpage_num);}
 
                   // handle special case of “c” and “e” instruction
                   if (instr == 'c') {
                       ctx_switches ++;
                       cur_proc = &proc_list->at(vpage_num); // point to cur process
                   }
                   else if (instr == 'e') {
                       process_exits ++;
                       process_exit(cur_proc);
                   }
                   // now the real instructions for read and write
                   else {  // r / w

                       PTE * pte = &(cur_proc->PT[vpage_num]); //point to cur instr's pte
 
                       set_up_pte(cur_proc, pte);
 
                       if ( ! pte->VALID) {   // page fault        
                          
                           // first determine that the vpage can be accessed
                           if (pte->isVMA){   // not a hole

                               frame_t * new_frame = get_frame();
                               
                               unmap(new_frame);
                               map(cur_proc, new_frame, pte, pgr);
                           }
                           // if not, a SEGV output line must be created
                           else {             // hole
                               cur_proc->statistics.segv++;
                               if (O) {std::cout << " SEGV" << std::endl;}
                           }
                       }
                       update_r_and_m_bits(cur_proc, instr, pte); // update_pte(read/modify) bits based on operations.
                   }
               }
           }  
   }
 
   /////////////////////////////////////////// OUTPUT ///////////////////////////////////////////
 
   // Per page table
   if (P) {
       for (int i = 0; i < proc_list->size(); i++){
           std::cout << "PT[" << i <<  "]:";
           for (int j = 0; j < PT_size; j++) {
               //proc_list->at(i).PT[j]
 
               if (!proc_list->at(i).PT[j].VALID){                  // non-valid
                   if (proc_list->at(i).PT[j].PAGEDOUT){
                       std::cout << " #";
                   }
                   else {
                       std::cout << " *";
                   }
               }
               else{                     
 
                   std::cout << " " << j << ":";                   // valid
 
                   if (proc_list->at(i).PT[j].REFERENCED){
                       std::cout << "R";
                   }
                   if (!proc_list->at(i).PT[j].REFERENCED){
                       std::cout << "-";
                   }
                   if (proc_list->at(i).PT[j].MODIFIED){
                       std::cout << "M";
                   }
                   if (!proc_list->at(i).PT[j].MODIFIED){
                       std::cout << "-";
                   }
                   if (proc_list->at(i).PT[j].PAGEDOUT){
                       std::cout << "S";
                   }
                   if (!proc_list->at(i).PT[j].PAGEDOUT){
                       std::cout << "-";
                   }
               }
           }
           std::cout << std::endl;
       }
   }
 
   // Print frame table : <pid:virtual page> or ‘*’ if not currently mapped by any virtual page
   if (F){
       std::cout << "FT:";
       for (int i = 0; i < frame_table->size(); i++){   // int i = 0 的 0不要省略
           if (frame_table->at(i).PT_pointer != nullptr) {
               std::cout << " " << frame_table->at(i).pid << ":" << frame_table->at(i).PT_pointer->vaddr;
           }
           else {
               std::cout << " *";
           }
       }
       std::cout << std::endl;
   }
  
   // Per process output
   if (S) {
       for (int i = 0; i < proc_list->size(); i++){
 
           cur_proc = &proc_list->at(i);
           pstats = &(cur_proc->statistics);
           // cost
           cost += (pstats->unmaps + pstats->maps) * 400;      // maps and unmaps each cost 400 cycles
           cost += (pstats->ins + pstats->outs) * 3000;        // page-in/outs each cost 3000 cycles
           cost += (pstats->fins + pstats->fouts) * 2500;      // file in/outs cost 2500 cycles
           cost += (pstats->zeros) * 150;                      // zeroing a page costs 150 cycles
           cost += (pstats->segv) * 240;                       // a segv costs 240 cycles
           cost += (pstats->segprot) * 300;                    // a segprot costs 300 cycles
 
           printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n",
                   cur_proc->pid,pstats->unmaps, pstats->maps, pstats->ins, pstats->outs,
                   pstats->fins, pstats->fouts, pstats->zeros, pstats->segv, pstats->segprot);
       }
 
 
       // Summary output
       cost += (inst_count - ctx_switches - process_exits) * 1; // each access (read or write) costs 1 cycles
       cost += ctx_switches * 121;                              // a context switch costs 121 cycles
       cost += process_exits * 175;                             // process exit costs 175 cycles
       printf("TOTALCOST %lu %lu %lu %llu\n", inst_count, ctx_switches, process_exits, cost);  // %llu long long unsigned
   }
 
   inputFile.close();
 
}
