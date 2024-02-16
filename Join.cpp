#include "Join.hpp"

#include <vector>

using namespace std;

/*
 * Input: Disk, Memory, Disk page ids for left relation, Disk page ids for right relation
 * Output: Vector of Buckets of size (MEM_SIZE_IN_PAGE - 1) after partition
 */
vector<Bucket> partition(Disk* disk, Mem* mem, pair<uint, uint> left_rel,
                         pair<uint, uint> right_rel) {
	// TODO: implement partition phase
	vector<Bucket> partitions(MEM_SIZE_IN_PAGE - 1, Bucket(disk));
	
	//hash right relationship
	for (uint r = right_rel.first; r < right_rel.second; r++) {
		mem->loadFromDisk(disk, r, MEM_SIZE_IN_PAGE - 1);
		//iterate through page
		for(uint ri = 0; ri < mem->mem_page(MEM_SIZE_IN_PAGE  - 1)->size(); ri++){
			//create hash value for key of the record
			uint hashVal = mem->mem_page(MEM_SIZE_IN_PAGE - 1)->get_record(ri).partition_hash() 
			% (MEM_SIZE_IN_PAGE - 1);
			//load individual record into buffer page
			mem->mem_page(hashVal)->loadRecord(mem->mem_page(MEM_SIZE_IN_PAGE - 1)->get_record(ri));
			//if bucket is full, flush to disk
			if(mem->mem_page(hashVal)->full()){
				partitions[hashVal].add_right_rel_page(mem->flushToDisk(disk, hashVal));
			}
		}
	}

	//flush right partition to disk
	for (uint r = 0; r < MEM_SIZE_IN_PAGE - 1; r++) {
		if (!(mem->mem_page(r)->empty())) {
			partitions[r].add_right_rel_page(mem->flushToDisk(disk, r));
		}
		
	}

	mem->reset();

	//hash left relationship
	for (uint l = left_rel.first; l < left_rel.second; l++) {
		mem->loadFromDisk(disk, l, MEM_SIZE_IN_PAGE - 1);
		//iterate through page
		for(uint li = 0; li < mem->mem_page(MEM_SIZE_IN_PAGE  - 1)->size(); li++){
			//create hash value for key of the record
			uint hashVal = mem->mem_page(MEM_SIZE_IN_PAGE - 1)->get_record(li).partition_hash() 
			% (MEM_SIZE_IN_PAGE - 1);
			//load individual record into buffer page
			mem->mem_page(hashVal)->loadRecord(mem->mem_page(MEM_SIZE_IN_PAGE - 1)->get_record(li));
			//if bucket is full, flush to disk
			if(mem->mem_page(hashVal)->full()){
				partitions[hashVal].add_left_rel_page(mem->flushToDisk(disk, hashVal));
			}
		}
	}
	

	//flush left partition to disk
	for (uint l = 0; l < MEM_SIZE_IN_PAGE - 1; l++) {
		if (!(mem->mem_page(l)->empty())) {
			partitions[l].add_left_rel_page(mem->flushToDisk(disk, l));
		}
	}
	mem->reset();
	
	return partitions;
}

/*
 * Input: Disk, Memory, Vector of Buckets after partition
 * Output: Vector of disk page ids for join result
 */
vector<uint> probe(Disk* disk, Mem* mem, vector<Bucket>& partitions) {
	// TODO: implement probe phase

	vector<uint> disk_pages; // placeholder
	Page* write = mem->mem_page(MEM_SIZE_IN_PAGE - 1);
	for (uint b = 0; b < partitions.size(); b++) {
		Bucket& buck = partitions[b];
		vector<uint> right;
		vector<uint> left;
		bool right_bigger = (buck.num_right_rel_record >= buck.num_left_rel_record); //true if right is bigger or equal, false if left is bigger
		if (right_bigger) {
			right = buck.get_right_rel();
			left = buck.get_left_rel();
		}
		else {
			right = buck.get_left_rel();
			left = buck.get_right_rel();
		}

		for (uint l = 0; l < left.size(); l++) {
			mem->loadFromDisk(disk, left[l], MEM_SIZE_IN_PAGE - 2);
			Page* readin = mem->mem_page(MEM_SIZE_IN_PAGE - 2);
			for (uint i = 0; i < readin->size(); i++) {
				uint hash = (readin->get_record(i)).probe_hash() 
				% (MEM_SIZE_IN_PAGE - 2);
				mem->mem_page(hash)->loadRecord(readin->get_record(i));
			}
		}

		for (uint r = 0; r < right.size(); r++) {
			mem->loadFromDisk(disk, right[r], MEM_SIZE_IN_PAGE - 2);
			Page* readin = mem->mem_page(MEM_SIZE_IN_PAGE - 2);
			for (uint i = 0; i < readin->size(); i++) {
				Record r = readin->get_record(i);
				uint hash = r.probe_hash() % (MEM_SIZE_IN_PAGE - 2);
				//mem->mem_page(hash)->loadRecord(r);

				Page* check = mem->mem_page(hash);
				for (uint j = 0; j < check->size(); j++) {
					if ((check->get_record(j) == r) && right_bigger) {
						write->loadPair(check->get_record(j), r);
					}
					else if ((check->get_record(j) == r) && !right_bigger) {
						write->loadPair(r, check->get_record(j));
					}
					if ((check->get_record(j) == r) && write->full()) {
						disk_pages.push_back(mem->flushToDisk(disk, MEM_SIZE_IN_PAGE - 1));
					}
				}
			}
		}

		//reset
		for(uint i = 0; i < MEM_SIZE_IN_PAGE - 1; i++){
			(mem->mem_page(i))->reset();
		}
	}

	//flush if write is not full
	if (!(write->full())) {
		disk_pages.push_back(mem->flushToDisk(disk, MEM_SIZE_IN_PAGE - 1));
	}
	return disk_pages;
}
