#include <iostream>
#include <list>
#include <unordered_map>
#include <cstdbool>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <algorithm>


#define USE_STD_ASSERT 1

#define _assert(exp) assert(exp)
		
typedef std::list<std::pair<int,int>>::iterator dl_it;
typedef std::list<std::pair<int, std::list<std::pair<int,int>>::iterator>>::iterator iter_list_it;

class LRUCache {
    
private:
	bool isFull(){
        if(map.size() >= _capacity)
            return true;
        else
            return false;
    }
	
    void updateCache(int key, int value){
    
		auto map_it = map.find(key);
		dl_it it = map_it->second->second;
		it->second = value;
		
		iter_list.erase(map_it->second);
		iter_list.push_front({key, it});
		map[key] = iter_list.begin();
    }
	
    void evictLastRecentAndAdd(int key, int value) {
		
        dl_it it = (--iter_list.end())->second;
		int _key = (--iter_list.end())->first;

		iter_list.pop_back();
		map.erase(map.find(_key)); 
		
		it->first = key; it->second = value;
		iter_list.push_front({key, it});
		map.insert({key, iter_list.begin()});
    }
	
protected:
	
    int _capacity;
	std::list<std::pair<int,int>> dl;
	std::list<std::pair<int, std::list<std::pair<int,int>>::iterator>> iter_list;
	std::unordered_map<int, std::list<std::pair<int, std::list<std::pair<int,int>>::iterator>>::iterator> map;
	
	
public:
    LRUCache(int capacity) {
		
        _capacity = capacity;
    }
    
    int get(int key) {
		
        auto map_it = map.find(key);
        if(map_it != map.end()){
			int val = map.find(key)->second->second->second;
			updateCache(key, val);
			auto map_it = map.find(key);
            return map_it->second->second->second;
        }else
            return -1;
	}
    
    int put(int key, int value) {
		
        auto map_it = map.find(key);
        if( map_it != map.end()){
            updateCache(key, value);
			return 1;
        }else
		{
            if( isFull() ){
                evictLastRecentAndAdd(key, value);
				return 2;
            }else{
                dl.push_front({key, value});
				iter_list.push_front({key, dl.begin()});
                map.insert({key, iter_list.begin()});
				return 3;
            }
		}
	}
};

class LRUCachePrinter : public LRUCache
{
	
	public:
	LRUCachePrinter(int capacity): LRUCache{capacity}
	{
	}
	void printdl() const
	{
		std::cout<<"dl: ";
		for(auto it = dl.begin(); it != dl.end(); ++it)
		{
			std::cout<<it->first<<"->"<<it->second<<" ";
		}
		std::cout<<std::endl;
	}
	
	void print_iterlist() const
	{
		std::cout<<"Iter list: ";
		for(auto it = iter_list.begin(); it != iter_list.end(); ++it)
		{
			std::cout<<it->second->first<<"->"<<it->second->second<<" ";
		}
		std::cout<<std::endl;
	}
	void printmap() const
	{
		std::cout<<"Map: ";
		for(auto it = map.begin(); it != map.end(); ++it)
		{
			std::cout<<it->first<<"->"<<it->second->second->first<<" ";
		}
		std::cout<<std::endl;
	}
	
	void printAll() const
	{
		printdl();
		print_iterlist();
		printmap();
		std::cout<<std::endl;
	}
};

double _time_test(int size)
{
	LRUCachePrinter *lru = new LRUCachePrinter(size);
	
	srand( time(NULL) );
	
	std::chrono::system_clock::time_point start = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < size * 10; ++i)
	{
		if( (rand() % 10) < 8 )
			lru->put( (int) rand() % (size * 100), (int) rand() % (size * 100) );
		else
			lru->get( (int) rand() % (size * 100) );
	}
	
	std::chrono::system_clock::time_point stop = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = stop - start;	
	
	delete lru;
	return elapsed.count() / (size * 10);
}

void complexity_test()
{
	std::cout<< _time_test(100) <<std::endl;
	std::cout<< _time_test(250) <<std::endl;
	std::cout<< _time_test(500) <<std::endl;
	std::cout<< _time_test(1000) <<std::endl;
	std::cout<< _time_test(2500) <<std::endl;
	std::cout<< _time_test(5000) <<std::endl;
	std::cout<< _time_test(10000) <<std::endl;
	std::cout<< _time_test(25000) <<std::endl;
	std::cout<< _time_test(50000) <<std::endl;
	std::cout<< _time_test(100000) <<std::endl;
	std::cout<< _time_test(500000) <<std::endl;

}

class LRU_Test : public LRUCache
{
private:

	std::list<std::pair<int, int>> test_list;
	
	int get_ret_val;
	int _capacity;
	int put_ret;
	
	int last_key, last_val, prev_last_key, prev_last_val;
	iter_list_it list_it_end, list_it_beg;
	
	int get_cnt = 0, get_not_found_cnt = 0, get_found_cnt = 0;
	int put_cnt = 0, put_update_cnt = 0, put_evict_cnt = 0;
	
	void preserve_last_recents()
	{
		if( dl.size() >= 1 ){
			//Preserve last recent elements, at(end) and at(end-1) in iter_list
			list_it_end = --iter_list.end();	
			if( iter_list.begin() != list_it_end )
			{
				prev_last_key = (--list_it_end)->second->first;
				prev_last_val = (--list_it_end)->second->second;
			}
			
			last_key = list_it_end->second->first;
			last_val = list_it_end->second->second;
		}
	}
	
public:
	LRU_Test(int capacity): LRUCache{capacity}, _capacity(capacity)
	{}
	
	void print_res()
	{
		std::cout<<"Put() : "<<put_cnt<<" (put_update: "<<put_update_cnt<<", put_evict: "<<put_evict_cnt<<")"<<std::endl;
		std::cout<<"Get() : "<<get_cnt<<" (get_not_found: "<<get_not_found_cnt<<", get_found: "<<get_found_cnt<<")"<<std::endl;
	}
	
	void test()
	{	
		srand(time(NULL));
		int loop_cnt = _capacity * (rand() % 100 + 1);
		float put_get_ratio = rand();
		
		for(int i = 1; i <= loop_cnt; ++i)
		{
			int key = rand() % (_capacity * 10);			
			int val = rand() % (_capacity * 10);
			std::pair<int, int> pair{-1, -1};
			
			//put() and get() randomly
			if( rand() >= put_get_ratio )
			{
				preserve_last_recents();
				
				put_cnt++;
				put_ret = put(key, val);

				if( dl.size() >= 1 ){
					if( map.find(key)->second == (--iter_list.end()) )
						preserve_last_recents();
				}

				//Update cache
				if(put_ret == 1 ){	
					int _key = key;
					for_each(test_list.begin(), test_list.end(), [_key, &pair](const std::pair<int,int> &p) { if(_key == p.first) {pair = p; return;} });	
					test_list.erase( std::find(test_list.begin(), test_list.end(), std::make_pair(key, std::get<1>(pair)) ));
					test_list.push_front({key, val});
					
				}else{
					test_list.push_front({key, val});
					if(test_list.size() > _capacity){
						test_list.pop_back();
					}
				}
			}else{
				get_cnt++;
				get_ret_val = get(key);
				if(get_ret_val != -1){
					test_list.erase( std::find(test_list.begin(), test_list.end(), std::make_pair(key, get_ret_val) ));
					test_list.push_front({key, get_ret_val});
					preserve_last_recents(); 
				}
				put_ret = -1;	
			}

			//Get() called
			if(put_ret < 0){
			
				int _key = key;
				for_each(test_list.begin(), test_list.end(), [_key, &pair](const std::pair<int,int> &p) { if(_key == p.first) {pair = p; return;} });
				
				_assert( std::get<1>(pair) == get_ret_val );
				
				if( get_ret_val != -1) {
					//If get element exist check if its on first position
					_assert( iter_list.front().first == key );
					//Check dl for key
					_assert( (map.find(key)->second->second->first == iter_list.begin()->second->first) \
							&& (iter_list.begin()->second->first == key) );
					//Check dl for val
					_assert( (map.find(key)->second->second->second == iter_list.begin()->second->second) \
							&& (iter_list.begin()->second->second == std::get<1>(pair)) );
					get_found_cnt++;
				}else
					get_not_found_cnt++;
			}else{

				list_it_beg = iter_list.begin();
				_assert( (key == list_it_beg->first) && (val == list_it_beg->second->second) );
				
				if(put_ret == 1){			//Update cache
					//No remove or add to list
					if( iter_list.begin() != --iter_list.end() ){
						list_it_end = --iter_list.end();
						//_assert( (last_key == list_it_end->first) && (list_it_end->first == list_it_end->second->first) );
						//_assert( (last_val == list_it_end->second->second) );
					}
					put_update_cnt++;
					
				}else if(put_ret == 2){ 	//Evict element
				
					list_it_end = --iter_list.end();
					
					_assert( (last_key != list_it_end->second->first) );
					//_assert( (last_val != list_it_end->second->second) );
					
					_assert( (prev_last_key == list_it_end->first) && (list_it_end->first == list_it_end->second->first) );
					
					//std::cout<< prev_last_val <<" "<< list_it_end->second->second <<std::endl;
					//_assert( (prev_last_val == list_it_end->second->second) );
					put_evict_cnt++;
					
				}else{
					//list_it_end = --iter_list.end();
					//_assert( (-1 == list_it_end->first) && (list_it_end->first == list_it_end->second->first) );
					//_assert( (-1 == list_it_end->second->second) );
				}
			}	

		}
	}
};


void test(int test_runs)
{	
	srand( time(NULL) );
	
	for(int i = 0; i < test_runs; ++i)
	{
		int size = rand() % 100 + 2;
		LRU_Test *lru = new LRU_Test(size);
		lru->test();
		lru->print_res();
		delete lru;
		std::cout<<std::endl;
		std::cout<<"Test: "<<i<<" size: "<<size<<std::endl;
	}
	std::cout<<"End test"<<std::endl;
}

void testX(LRUCache *lru)
{
	std::cout<<"Start test..."<<std::endl;
	lru->put(33,219);
	assert( lru->get(39) == -1 );
	lru->put(96,56);
	assert( lru->get(129) == -1 );
	assert( lru->get(115) == -1 );
	assert( lru->get(112) == -1 );
	lru->put(3,280);
	assert( lru->get(40) == -1 );
	lru->put(85,193);
	lru->put(10,10);
	lru->put(100,136);
	lru->put(12,66);
	lru->put(81,261);
	lru->put(33,58);
	assert( lru->get(3) == -1 );
	lru->put(121,308);
	lru->put(129,263);
	assert( lru->get(105) == -1 );
	lru->put(104,38);
	lru->put(65,85);
	lru->put(3,141);
	lru->put(29,30);
	lru->put(80,191);
	lru->put(52,191);
	lru->put(8,300);
	assert( lru->get(136) == -1 );
	lru->put(48,261);
	lru->put(3,193);
	lru->put(133,193);
	lru->put(60,183);
	lru->put(128,148);
	lru->put(52,176);
	assert( lru->get(48) == -1 );
	lru->put(48,119);
	lru->put(10,241);
	assert( lru->get(124) == -1 );
	lru->put(130,127);
	assert( lru->get(61) == -1 );
	lru->put(124,27);
	assert( lru->get(94) == -1 );
	lru->put(29,304);
	lru->put(102,314);
	assert( lru->get(110) == -1 );
	lru->put(23,49);
	lru->put(134,12);
	lru->put(55,90);
	assert( lru->get(14) == -1 );
	assert( lru->get(104) == -1 );
	lru->put(77,165);
	lru->put(60,160);
	assert( lru->get(117) == -1 );
	lru->put(58,30);
	assert( lru->get(54) == -1 );
	assert( lru->get(136) == -1 );
	assert( lru->get(128) == -1 );
	assert( lru->get(131) == -1 );
	lru->put(48,114);
	assert( lru->get(136) == -1 );
	lru->put(46,51);
	lru->put(129,291);
	lru->put(96,207);
	assert( lru->get(131) == -1 );
	lru->put(89,153);
	lru->put(120,154);
	assert( lru->get(111) == -1 );
	assert( lru->get(47) == -1 );
	assert( lru->get(5) == -1 );
	lru->put(114,157);
	lru->put(57,82);
	lru->put(113,106);
	lru->put(74,208);
	assert( lru->get(56) == -1 );
	assert( lru->get(59) == -1 );
	assert( lru->get(100) == -1 );
	assert( lru->get(132) == -1 );
	lru->put(127,202);
	assert( lru->get(75) == -1 );
	lru->put(102,147);
	assert( lru->get(37) == -1 );
	lru->put(53,79);
	lru->put(119,220);
	assert( lru->get(47) == -1 );
	assert( lru->get(101) == -1 );
	assert( lru->get(89) == -1 );
	assert( lru->get(20) == -1 );
	assert( lru->get(93) == -1 );
	assert( lru->get(7) == -1 );
	lru->put(48,109);
	lru->put(71,146);
	assert( lru->get(43) == -1 );
	assert( lru->get(122) == -1 );
	lru->put(3,160);
	assert( lru->get(17) == -1 );
	lru->put(80,22);
	lru->put(80,272);
	assert( lru->get(75) == -1 );
	assert( lru->get(117) == -1 );
	lru->put(76,204);
	lru->put(74,141);
	lru->put(107,93);
	lru->put(34,280);
	lru->put(31,94);
	assert( lru->get(132) == -1 );
	lru->put(71,258);
	assert( lru->get(61) == -1 );
	assert( lru->get(60) == -1 );
	lru->put(69,272);
	assert( lru->get(46) == -1 );
	lru->put(42,264);
	lru->put(87,126);
	lru->put(107,236);
	lru->put(131,218);
	assert( lru->get(79) == -1 );
	lru->put(41,71);
	lru->put(94,111);
	lru->put(19,124);
	lru->put(52,70);
	assert( lru->get(131) == -1 );
	assert( lru->get(103) == -1 );
	assert( lru->get(81) == -1 );
	assert( lru->get(126) == -1 );
	lru->put(61,279);
	lru->put(37,100);
	assert( lru->get(95) == -1 );
	assert( lru->get(54) == -1 );
	lru->put(59,136);
	lru->put(101,219);
	lru->put(15,248);
	lru->put(37,91);
	lru->put(11,174);
	lru->put(99,65);
	lru->put(105,249);
	assert( lru->get(85) == -1 );
	lru->put(108,287);
	lru->put(96,4);
	assert( lru->get(70) == -1 );
	assert( lru->get(24) == -1 );
	lru->put(52,206);
	lru->put(59,306);
	lru->put(18,296);
	lru->put(79,95);
	lru->put(50,131);
	lru->put(3,161);
	lru->put(2,229);
	lru->put(39,183);
	lru->put(90,225);
	lru->put(75,23);
	lru->put(136,280);
	assert( lru->get(119) == -1 );
	lru->put(81,272);
	assert( lru->get(106) == -1 );
	assert( lru->get(106) == -1 );
	assert( lru->get(70) == -1 );
	lru->put(73,60);
	lru->put(19,250);
	lru->put(82,291);
	lru->put(117,53);
	lru->put(16,176);
	assert( lru->get(40) == -1 );
	lru->put(7,70);
	lru->put(135,212);
	assert( lru->get(59) == -1 );
	lru->put(81,201);
	lru->put(75,305);
	assert( lru->get(101) == -1 );
	lru->put(8,250);
	assert( lru->get(38) == -1 );
	lru->put(28,220);
	assert( lru->get(21) == -1 );
	lru->put(105,266);
	assert( lru->get(105) == 266 );
	assert( lru->get(85) == -1 );
	assert( lru->get(55) == -1 );
	assert( lru->get(6) == -1 );
	lru->put(78,83);
	assert( lru->get(126) == -1 );
	assert( lru->get(102) == -1 );
	assert( lru->get(66) == -1 );
	lru->put(61,42);
	lru->put(127,35);
	lru->put(117,105);
	assert( lru->get(128) == -1 );
	assert( lru->get(102) == -1 );
	assert( lru->get(50) == -1 );
	lru->put(24,133);
	lru->put(40,178);
	lru->put(78,157);
	lru->put(71,22);
	assert( lru->get(25) == -1 );
	assert( lru->get(82) == -1 );
	assert( lru->get(129) == -1 );
	lru->put(126,12);
	assert( lru->get(45) == -1 );
	assert( lru->get(40) == 178 );
	assert( lru->get(86) == -1 );
	assert( lru->get(100) == -1 );
	lru->put(30,110);
	assert( lru->get(49) == -1 );
	lru->put(47,185);
	lru->put(123,101);
	assert( lru->get(102) == -1 );
	assert( lru->get(5) == -1 );
	lru->put(40,267);
	lru->put(48,155);
	assert( lru->get(108) == -1 );
	assert( lru->get(45) == -1 );
	lru->put(14,182);
	lru->put(20,117);
	lru->put(43,124);
	assert( lru->get(38) == -1 );
	lru->put(77,158);
	assert( lru->get(111) == -1 );
	assert( lru->get(39) == -1 );
	lru->put(69,126);
	lru->put(113,199);
	lru->put(21,216);
	assert( lru->get(11) == -1 );
	lru->put(117,207);
	assert( lru->get(30) == -1 );
	lru->put(97,84);
	assert( lru->get(109) == -1 );
	lru->put(99,218);
	assert( lru->get(109) == -1 );
	lru->put(113,1);
	assert( lru->get(62) == -1 );
	lru->put(49,89);
	lru->put(53,311);
	assert( lru->get(126) == -1 );
	lru->put(32,153);
	lru->put(14,296);
	assert( lru->get(22) == -1 );
	lru->put(14,225);
	assert( lru->get(49) == 89 );
	assert( lru->get(75) == -1 );
	lru->put(61,241);
	assert( lru->get(7) == -1 );
	assert( lru->get(6) == -1 );
	assert( lru->get(31) == -1 );
	lru->put(75,15);
	assert( lru->get(115) == -1 );
	lru->put(84,181);
	lru->put(125,111);
	lru->put(105,94);
	lru->put(48,294);
	assert( lru->get(106) == -1 );
	assert( lru->get(61) == -1 );
	lru->put(53,190);
	assert( lru->get(16) == -1 );
	lru->put(12,252);
	assert( lru->get(28) == -1 );
	lru->put(111,122);
	assert( lru->get(122) == -1 );
	lru->put(10,21);
	assert( lru->get(59) == -1 );
	assert( lru->get(72) == -1 );
	assert( lru->get(39) == -1 );
	assert( lru->get(6) == -1 );
	assert( lru->get(126) == -1 );
	lru->put(131,177);
	lru->put(105,253);
	assert( lru->get(26) == -1 );
	lru->put(43,311);
	assert( lru->get(79) == -1 );
	lru->put(91,32);
	lru->put(7,141);
	assert( lru->get(38) == -1 );
	assert( lru->get(13) == -1 );
	lru->put(79,135);
	assert( lru->get(43) == 311 );
	assert( lru->get(94) == -1 );
	lru->put(80,182);
	assert( lru->get(53) == -1 );
	lru->put(120,309);
	lru->put(3,109);
	assert( lru->get(97) == -1 );
	lru->put(9,128);
	lru->put(114,121);
	assert( lru->get(56) == -1 );
	assert( lru->get(56) == -1 );
	lru->put(124,86);
	lru->put(34,145);
	assert( lru->get(131) == -1 );
	assert( lru->get(78) == -1 );
	lru->put(86,21);
	assert( lru->get(98) == -1 );
	lru->put(115,164);
	lru->put(47,225);
	assert( lru->get(95) == -1 );
	lru->put(89,55);
	lru->put(26,134);
	lru->put(8,15);
	assert( lru->get(11) == -1 );
	lru->put(84,276);
	lru->put(81,67);
	assert( lru->get(46) == -1 );
	assert( lru->get(39) == -1 );
	assert( lru->get(92) == -1 );
	assert( lru->get(96) == -1 );
	lru->put(89,51);
	lru->put(136,240);
	assert( lru->get(45) == -1 );
	assert( lru->get(27) == -1 );
	lru->put(24,209);
	lru->put(82,145);
	assert( lru->get(10) == -1 );
	lru->put(104,225);
	lru->put(120,203);
	lru->put(121,108);
	lru->put(11,47);
	assert( lru->get(89) == -1 );
	lru->put(80,66);
	assert( lru->get(16) == -1 );
	lru->put(95,101);
	assert( lru->get(49) == -1 );
	assert( lru->get(1) == -1 );
	lru->put(77,184);
	assert( lru->get(27) == -1 );
	lru->put(74,313);
	lru->put(14,118);
	assert( lru->get(16) == -1 );
	assert( lru->get(74) == 313 );
	lru->put(88,251);
	assert( lru->get(124) == -1 );
	lru->put(58,101);
	lru->put(42,81);
	assert( lru->get(2) == -1 );
	lru->put(133,101);
	assert( lru->get(16) == -1 );
	lru->put(1,254);
	lru->put(25,167);
	lru->put(53,56);
	lru->put(73,198);
	assert( lru->get(48) == -1 );
	assert( lru->get(30) == -1 );
	assert( lru->get(95) == -1 );
	lru->put(90,102);
	lru->put(92,56);
	lru->put(2,130);
	lru->put(52,11);
	assert( lru->get(9) == -1 );
	assert( lru->get(23) == -1 );
	lru->put(53,275);
	lru->put(23,258);
	assert( lru->get(57) == -1 );
	lru->put(136,183);
	lru->put(75,265);
	assert( lru->get(85) == -1 );
	lru->put(68,274);
	lru->put(15,255);
	assert( lru->get(85) == -1 );
	lru->put(33,314);
	lru->put(101,223);
	lru->put(39,248);
	lru->put(18,261);
	lru->put(37,160);
	assert( lru->get(112) == -1 );
	assert( lru->get(65) == -1 );
	lru->put(31,240);
	lru->put(40,295);
	lru->put(99,231);
	assert( lru->get(123) == -1 );
	lru->put(34,43);
	assert( lru->get(87) == -1 );
	assert( lru->get(80) == -1 );
	lru->put(47,279);
	lru->put(89,299);
	assert( lru->get(72) == -1 );
	lru->put(26,277);
	lru->put(92,13);
	lru->put(46,92);
	lru->put(67,163);
	lru->put(85,184);
	assert( lru->get(38) == -1 );
	lru->put(35,65);
	assert( lru->get(70) == -1 );
	assert( lru->get(81) == -1 );
	lru->put(40,65);
	assert( lru->get(80) == -1 );
	lru->put(80,23);
	lru->put(76,258);
	assert( lru->get(69) == -1 );
	assert( lru->get(133) == -1 );
	lru->put(123,196);
	lru->put(119,212);
	lru->put(13,150);
	lru->put(22,52);
	lru->put(20,105);
	lru->put(61,233);
	assert( lru->get(97) == -1 );
	lru->put(128,307);
	assert( lru->get(85) == -1 );
	assert( lru->get(80) == -1 );
	assert( lru->get(73) == -1 );
	assert( lru->get(30) == -1 );
	lru->put(46,44);
	assert( lru->get(95) == -1 );
	lru->put(121,211);
	lru->put(48,307);
	assert( lru->get(2) == -1 );
	lru->put(27,166);
	assert( lru->get(50) == -1 );
	lru->put(75,41);
	lru->put(101,105);
	assert( lru->get(2) == -1 );
	lru->put(110,121);
	lru->put(32,88);
	lru->put(75,84);
	lru->put(30,165);
	lru->put(41,142);
	lru->put(128,102);
	lru->put(105,90);
	lru->put(86,68);
	lru->put(13,292);
	lru->put(83,63);
	lru->put(5,239);
	assert( lru->get(5) == 239 );
	lru->put(68,204);
	assert( lru->get(127) == -1 );
	lru->put(42,137);
	assert( lru->get(93) == -1 );
	lru->put(90,258);
	lru->put(40,275);
	lru->put(7,96);
	assert( lru->get(108) == -1 );
	lru->put(104,91);
	assert( lru->get(63) == -1 );
	assert( lru->get(31) == -1 );
	lru->put(31,89);
	assert( lru->get(74) == -1 );
	assert( lru->get(81) == -1 );
	lru->put(126,148);
	assert( lru->get(107) == -1 );
	lru->put(13,28);
	lru->put(21,139);
	assert( lru->get(114) == -1 );
	assert( lru->get(5) == -1 );
	assert( lru->get(89) == -1 );
	assert( lru->get(133) == -1 );
	assert( lru->get(20) == -1 );
	lru->put(96,135);
	lru->put(86,100);
	lru->put(83,75);
	assert( lru->get(14) == -1 );
	lru->put(26,195);
	assert( lru->get(37) == -1 );
	lru->put(1,287);
	assert( lru->get(79) == -1 );
	assert( lru->get(15) == -1 );
	assert( lru->get(6) == -1 );
	lru->put(68,11);
	assert( lru->get(52) == -1 );
	lru->put(124,80);
	lru->put(123,277);
	lru->put(99,281);
	assert( lru->get(133) == -1 );
	assert( lru->get(90) == -1 );
	assert( lru->get(45) == -1 );
	assert( lru->get(127) == -1 );
	lru->put(9,68);
	lru->put(123,6);
	lru->put(124,251);
	lru->put(130,191);
	lru->put(23,174);
	lru->put(69,295);
	assert( lru->get(32) == -1 );
	assert( lru->get(37) == -1 );
	lru->put(1,64);
	lru->put(48,116);
	assert( lru->get(68) == -1 );
	lru->put(117,173);
	lru->put(16,89);
	assert( lru->get(84) == -1 );
	lru->put(28,234);
	assert( lru->get(129) == -1 );
	assert( lru->get(89) == -1 );
	assert( lru->get(55) == -1 );
	assert( lru->get(83) == -1 );
	lru->put(99,264);
	assert( lru->get(129) == -1 );
	assert( lru->get(84) == -1 );
	assert( lru->get(14) == -1 );
	lru->put(26,274);
	assert( lru->get(109) == -1 );
	assert( lru->get(110) == -1 );
	lru->put(96,120);
	lru->put(128,207);
	assert( lru->get(12) == -1 );
	lru->put(99,233);
	lru->put(20,305);
	lru->put(26,24);
	lru->put(102,32);
	assert( lru->get(82) == -1 );
	lru->put(16,30);
	lru->put(5,244);
	assert( lru->get(130) == -1 );
	lru->put(109,36);
	lru->put(134,162);
	lru->put(13,165);
	lru->put(45,235);
	lru->put(112,80);
	assert( lru->get(6) == -1 );
	lru->put(34,98);
	lru->put(64,250);
	lru->put(18,237);
	lru->put(72,21);
	lru->put(42,105);
	lru->put(57,108);
	lru->put(28,229);
	assert( lru->get(83) == -1 );
	lru->put(1,34);
	lru->put(93,151);
	lru->put(132,94);
	lru->put(18,24);
	lru->put(57,68);
	lru->put(42,137);
	assert( lru->get(35) == -1 );
	assert( lru->get(80) == -1 );
	lru->put(10,288);
	assert( lru->get(21) == -1 );
	assert( lru->get(115) == -1 );
	assert( lru->get(131) == -1 );
	assert( lru->get(30) == -1 );
	assert( lru->get(43) == -1 );
	lru->put(97,262);
	lru->put(55,146);
	lru->put(81,112);
	lru->put(2,212);
	lru->put(5,312);
	lru->put(82,107);
	lru->put(14,151);
	assert( lru->get(77) == -1 );
	lru->put(60,42);
	lru->put(90,309);
	assert( lru->get(90) == 309 );
	lru->put(131,220);
	assert( lru->get(86) == -1 );
	lru->put(106,85);
	lru->put(85,254);
	assert( lru->get(14) == -1 );
	lru->put(66,262);
	lru->put(88,243);
	assert( lru->get(3) == -1 );
	lru->put(50,301);
	lru->put(118,91);
	assert( lru->get(25) == -1 );
	assert( lru->get(105) == -1 );
	assert( lru->get(100) == -1 );
	assert( lru->get(89) == -1 );
	lru->put(111,152);
	lru->put(65,24);
	lru->put(41,264);
	assert( lru->get(117) == -1 );
	assert( lru->get(117) == -1 );
	lru->put(80,45);
	assert( lru->get(38) == -1 );
	lru->put(11,151);
	lru->put(126,203);
	lru->put(128,59);
	lru->put(6,129);
	assert( lru->get(91) == -1 );
	lru->put(118,2);
	lru->put(50,164);
	assert( lru->get(74) == -1 );
	assert( lru->get(80) == -1 );
	lru->put(48,308);
	lru->put(109,82);
	lru->put(3,48);
	lru->put(123,10);
	lru->put(59,249);
	lru->put(128,64);
	lru->put(41,287);
	lru->put(52,278);
	lru->put(98,151);
	assert( lru->get(12) == -1 );
	assert( lru->get(25) == -1 );
	lru->put(18,254);
	lru->put(24,40);
	assert( lru->get(119) == -1 );
	lru->put(66,44);
	lru->put(61,19);
	lru->put(80,132);
	lru->put(62,111);
	assert( lru->get(80) == 132 );
	lru->put(57,188);
	assert( lru->get(132) == -1 );
	assert( lru->get(42) == -1 );
	lru->put(18,314);
	assert( lru->get(48) == -1 );
	lru->put(86,138);
	assert( lru->get(8) == -1 );
	lru->put(27,88);
	lru->put(96,178);
	lru->put(17,104);
	lru->put(112,86);
	assert( lru->get(25) == -1 );
	lru->put(129,119);
	lru->put(93,44);
	assert( lru->get(115) == -1 );
	lru->put(33,36);
	lru->put(85,190);
	assert( lru->get(10) == -1 );
	lru->put(52,182);
	lru->put(76,182);
	assert( lru->get(109) == -1 );
	assert( lru->get(118) == -1 );
	lru->put(82,301);
	lru->put(26,158);
	assert( lru->get(71) == -1 );
	lru->put(108,309);
	lru->put(58,132);
	lru->put(13,299);
	lru->put(117,183);
	assert( lru->get(115) == -1 );
	assert( lru->get(89) == -1 );
	assert( lru->get(42) == -1 );
	lru->put(11,285);
	lru->put(30,144);
	assert( lru->get(69) == -1 );
	lru->put(31,53);
	assert( lru->get(21) == -1 );
	lru->put(96,162);
	lru->put(4,227);
	lru->put(77,120);
	lru->put(128,136);
	assert( lru->get(92) == -1 );
	lru->put(119,208);
	lru->put(87,61);
	lru->put(9,40);
	lru->put(48,273);
	assert( lru->get(95) == -1 );
	assert( lru->get(35) == -1 );
	lru->put(62,267);
	lru->put(88,161);
	assert( lru->get(59) == -1 );
	assert( lru->get(85) == -1 );
	lru->put(131,53);
	lru->put(114,98);
	lru->put(90,257);
	lru->put(108,46);
	assert( lru->get(54) == -1 );
	lru->put(128,223);
	lru->put(114,168);
	lru->put(89,203);
	assert( lru->get(100) == -1 );
	assert( lru->get(116) == -1 );
	assert( lru->get(14) == -1 );
	lru->put(61,104);
	lru->put(44,161);
	lru->put(60,132);
	lru->put(21,310);
	assert( lru->get(89) == -1 );
	lru->put(109,237);
	assert( lru->get(105) == -1 );
	assert( lru->get(32) == -1 );
	lru->put(78,101);
	lru->put(14,71);
	lru->put(100,47);
	lru->put(102,33);
	lru->put(44,29);
	assert( lru->get(85) == -1 );
	assert( lru->get(37) == -1 );
	lru->put(68,175);
	lru->put(116,182);
	lru->put(42,47);
	assert( lru->get(9) == -1 );
	lru->put(64,37);
	lru->put(23,32);
	lru->put(11,124);
	lru->put(130,189);
	assert( lru->get(65) == -1 );
	lru->put(33,219);
	lru->put(79,253);
	assert( lru->get(80) == -1 );
	assert( lru->get(16) == -1 );
	lru->put(38,18);
	lru->put(35,67);
	assert( lru->get(107) == -1 );
	assert( lru->get(88) == -1 );
	lru->put(37,13);
	lru->put(71,188);
	assert( lru->get(35) == 67 ); 
	lru->put(58,268);
	lru->put(18,260);
	lru->put(73,23);
	lru->put(28,102);
	assert( lru->get(129) == -1 );
	assert( lru->get(88) == -1 );
	assert( lru->get(65) == -1 );
	assert( lru->get(80) == -1 );
	lru->put(119,146);
	assert( lru->get(113) == -1 );
	assert( lru->get(62) == -1 );
	lru->put(123,138);
	lru->put(18,1);
	lru->put(26,208);
	assert( lru->get(107) == -1 );
	assert( lru->get(107) == -1 );
	lru->put(76,132);
	lru->put(121,191);
	assert( lru->get(4) == -1 );
	assert( lru->get(8) == -1 );
	assert( lru->get(117) == -1 );
	lru->put(11,118);
	assert( lru->get(43) == -1 );
	assert( lru->get(69) == -1 );
	assert( lru->get(136) == -1 );
	lru->put(66,298);
	assert( lru->get(25) == -1 );
	assert( lru->get(71) == -1 );
	assert( lru->get(100) == -1 );
	lru->put(26,141);
	lru->put(53,256);
	lru->put(111,205);
	lru->put(126,106);
	assert( lru->get(43) == -1 ); 
	lru->put(14,39);
	lru->put(44,41);
	lru->put(23,230);
	assert( lru->get(131) == -1 );
	assert( lru->get(53) == -1 );
	lru->put(104,268);
	assert( lru->get(30) == -1 );
	lru->put(108,48);
	lru->put(72,45);
	assert( lru->get(58) == -1 );
	assert( lru->get(46) == -1 );
	lru->put(128,301);
	assert( lru->get(71) == -1 );
	assert( lru->get(99) == -1 );
	assert( lru->get(113) == -1 );
	assert( lru->get(121) == -1 );
	lru->put(130,122);
	lru->put(102,5);
	lru->put(111,51);
	lru->put(85,229);
	lru->put(86,157);
	lru->put(82,283);
	lru->put(88,52);
	lru->put(136,105);
	assert( lru->get(40) == -1 );
	assert( lru->get(63) == -1 );
	lru->put(114,244);
	lru->put(29,82);
	lru->put(83,278);
	assert( lru->get(131) == -1 );
	lru->put(56,33);
	assert( lru->get(123) == -1 );
	assert( lru->get(11) == -1 );
	assert( lru->get(119) == -1 );
	lru->put(119,1);
	lru->put(48,52);
	assert( lru->get(47) == -1 );
	lru->put(127,136);
	lru->put(78,38);
	lru->put(117,64);
	lru->put(130,134);
	lru->put(93,69);
	lru->put(70,98);
	assert( lru->get(68) == -1 );
	lru->put(4,3);
	lru->put(92,173);
	lru->put(114,65);
	lru->put(7,309);
	assert( lru->get(31) == -1 );
	lru->put(107,271);
	lru->put(110,69);	
	assert( lru->get(45) == -1 );
	lru->put(35,288);
	assert( lru->get(20) == -1 );
	lru->put(38,79);
	assert( lru->get(46) == -1 );
	lru->put(6,123);
	assert( lru->get(19) == -1 );
	lru->put(84,95);
	assert( lru->get(76) == -1 );
	lru->put(71,31);
	lru->put(72,171);
	lru->put(35,123);
	assert( lru->get(32) == -1 );
	lru->put(73,85);
	assert( lru->get(94) == -1 );
	assert( lru->get(128) == -1 );
	assert( lru->get(28) == -1 );
	assert( lru->get(38) == -1 );
	assert( lru->get(109) == -1 );
	lru->put(85,197);
	lru->put(10,41);
	lru->put(71,50);
	assert( lru->get(128) == -1 );
	lru->put(3,55);
	lru->put(15,9);
	lru->put(127,215);
	assert( lru->get(17) == -1 );
	assert( lru->get(37) == -1 );
	lru->put(111,272);
	lru->put(79,169);
	lru->put(86,206);
	lru->put(40,264);
	assert( lru->get(134) == -1 );
	lru->put(16,207);
	lru->put(27,127);
	lru->put(29,48);
	lru->put(32,122);
	lru->put(15,35);
	lru->put(117,36);
	assert( lru->get(127) == -1 );
	assert( lru->get(36) == -1 );
	lru->put(72,70);
	lru->put(49,201);
	lru->put(89,215);
	lru->put(134,290);
	lru->put(77,64);
	lru->put(26,101);
	assert( lru->get(99) == -1 );
	lru->put(36,96);
	lru->put(84,129);
	lru->put(125,264);
	assert( lru->get(43) == -1 );
	assert( lru->get(38) == -1 );
	lru->put(24,76);
	lru->put(45,2);
	lru->put(32,24);
	lru->put(84,235);
	lru->put(16,240);
	lru->put(17,289);
	lru->put(49,94);
	lru->put(90,54);
	lru->put(88,199);
	assert( lru->get(23) == -1 );
	lru->put(87,19);
	lru->put(11,19);
	assert( lru->get(24) == -1 );
	assert( lru->get(57) == -1 );
	assert( lru->get(4) == -1 );
	assert( lru->get(40) == -1 );
	lru->put(133,286);
	lru->put(127,231);
	assert( lru->get(51) == -1 );
	lru->put(52,196);
	assert( lru->get(27) == -1 );
	assert( lru->get(10) == -1 );
	assert( lru->get(93) == -1 );
	lru->put(115,143);
	lru->put(62,64);
	lru->put(59,200);
	lru->put(75,85);
	lru->put(7,93);
	lru->put(117,270);
	lru->put(116,6);
	assert( lru->get(32) == -1 );
	assert( lru->get(135) == -1 );
	lru->put(2,140);
	lru->put(23,1);
	lru->put(11,69);
	lru->put(89,30);
	lru->put(27,14);
	assert( lru->get(100) == -1 );
	assert( lru->get(61) == -1 );
	lru->put(99,41);
	lru->put(88,12);
	assert( lru->get(41) == -1 );
	lru->put(52,203);
	assert( lru->get(65) == -1 );
	lru->put(62,78);
	lru->put(104,276);
	lru->put(105,307);
	assert( lru->get(7) == -1 );
	lru->put(23,123);
	assert( lru->get(22) == -1 );
	lru->put(35,299);
	assert( lru->get(69) == -1 );
	assert( lru->get(11) == -1 );
	lru->put(14,112);
	assert( lru->get(115) == -1 );
	assert( lru->get(112) == -1 );
	assert( lru->get(108) == -1 );
	lru->put(110,165);
	lru->put(83,165);
	lru->put(36,260);
	lru->put(54,73);
	assert( lru->get(36) == 260 );
	lru->put(93,69);
	assert( lru->get(134) == -1 );
	lru->put(125,96);
	lru->put(74,127);
	lru->put(110,305);
	lru->put(92,309);
	lru->put(87,45);
	lru->put(31,266);
	assert( lru->get(10) == -1 );
	lru->put(114,206);
	lru->put(49,141);
	assert( lru->get(82) == -1 );
	lru->put(92,3);
	lru->put(91,160);
	assert( lru->get(41) == -1 );
	lru->put(60,147);
	lru->put(36,239);
	lru->put(23,296);
	lru->put(134,120);
	assert( lru->get(6) == -1 );
	lru->put(5,283);
	lru->put(117,68);
	assert( lru->get(35) == -1 );
	assert( lru->get(120) == -1 );
	lru->put(44,191);
	lru->put(121,14);
	lru->put(118,113);
	lru->put(84,106);
	assert( lru->get(23) == -1 );
	lru->put(15,240);
	assert( lru->get(37) == -1 );
	lru->put(52,256);
	lru->put(119,116);
	lru->put(101,7);
	lru->put(14,157);
	lru->put(29,225);
	lru->put(4,247);
	lru->put(8,112);
	lru->put(8,189);
	lru->put(96,220);
	assert( lru->get(104) == -1 );
	lru->put(72,106);
	lru->put(23,170);
	lru->put(67,209);
	lru->put(70,39);
	assert( lru->get(18) == -1 );
	assert( lru->get(6) == -1 );
	assert( lru->get(34) == -1 );
	lru->put(121,157);
	assert( lru->get(16) == -1 );
	assert( lru->get(19) == -1 );
	lru->put(83,283);
	lru->put(13,22);
	lru->put(33,143);
	lru->put(88,133);
	assert( lru->get(88) == 133 );
	lru->put(5,49);
	assert( lru->get(38) == -1 );
	assert( lru->get(110) == -1 );
	assert( lru->get(67) == -1 );
	lru->put(23,227);
	assert( lru->get(68) == -1 );
	assert( lru->get(3) == -1 );
	lru->put(27,265);
	assert( lru->get(31) == -1 );
	lru->put(13,103);
	assert( lru->get(116) == -1 );
	lru->put(111,282);
	lru->put(43,71);
	assert( lru->get(134) == -1 );
	lru->put(70,141);
	assert( lru->get(14) == -1 );
	assert( lru->get(119) == -1 );
	assert( lru->get(43) == 71 );
	assert( lru->get(122) == -1 );
	lru->put(38,187);
	lru->put(8,9);
	assert( lru->get(63) == -1 );
	lru->put(42,140);
	assert( lru->get(83) == -1 );
	assert( lru->get(92) == -1 );
	assert( lru->get(106) == -1 );
	assert( lru->get(28) == -1 );
	lru->put(57,139);
	lru->put(36,257);
	lru->put(30,204);
	assert( lru->get(72) == -1 );
	lru->put(105,243);
	assert( lru->get(16) == -1 );
	lru->put(74,25);
	assert( lru->get(22) == -1 );
	lru->put(118,144);
	assert( lru->get(133) == -1 );
	assert( lru->get(71) == -1 );
	lru->put(99,21);
	assert( lru->get(26) == -1 );
	assert( lru->get(35) == -1 );
	lru->put(89,209);
	lru->put(106,158);
	lru->put(76,63);
	lru->put(112,216);
	assert( lru->get(128) == -1 );
	assert( lru->get(54) == -1 );
	lru->put(16,165);
	lru->put(76,206);
	lru->put(69,253);
	assert( lru->get(23) == -1 );
	lru->put(54,111);
	assert( lru->get(80) == -1 );
	lru->put(111,72);
	lru->put(95,217);
	assert( lru->get(118) == -1 );
	lru->put(4,146);
	assert( lru->get(47) == -1 );
	lru->put(108,290);
	assert( lru->get(43) == -1 );
	lru->put(70,8);
	assert( lru->get(117) == -1 );
	assert( lru->get(121) == -1 );
	lru->put(42,220);
	assert( lru->get(48) == -1 );
	assert( lru->get(32) == -1 );
	lru->put(68,213);
	lru->put(30,157);
	lru->put(62,68);
	assert( lru->get(58) == -1 );
	lru->put(125,283);
	lru->put(132,45);
	assert( lru->get(85) == -1 );
	assert( lru->get(92) == -1 );
	lru->put(23,257);
	assert( lru->get(74) == -1 );
	lru->put(18,256);
	assert( lru->get(90) == -1 );
	lru->put(10,158);
	lru->put(57,34);
	assert( lru->get(27) == -1 );
	assert( lru->get(107) == -1 );
	
	std::cout<<"PASSED!"<<std::endl;
}

int main()
{
	std::cout<<"lru"<<std::endl;
	LRUCachePrinter *lru = new LRUCachePrinter(4);
	//testX(lru);
	//complexity_test();
	test(1000);
	
	
	//lru->put(2,10); lru->printAll();
	//lru->put(5,8); lru->printAll();
	//lru->put(1,6); lru->printAll();
	//lru->put(6,13); lru->printAll();
	//lru->put(30,1); lru->printAll();
	//lru->put(9, 4); lru->printAll();
	//lru->put(18, 40); lru->printAll();
	//lru->put(0, 4); lru->printAll();
	
	delete lru;
return 0;
}