#ifndef CPU_HPP
#define CPU_HPP

#include <array>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <vector>

class Memory { 
	private: 
		static constexpr std::size_t RAM_SIZE = 128*1024*1024; 
		std::vector<uint8_t> memory_; // store memory on heap s
							   // program doesn't hit 
							   // thread stack limit 
		static const uint32_t BASE_OFFSET = 0x80000000;// standard mem start
		//helper for load functions, convert standard addr to vector addr
		std::size_t translate_address(uint32_t raddress) const;
			
	public:
		Memory();
		//load word, halfword, byte, signed/unsigned
		//byte, load whatever's at given memory address into destination
		int32_t load_byte(uint32_t addr) const;
		uint32_t load_ubyte(uint32_t addr) const;

		//load halfword signed 
		int32_t load_hws(uint32_t addr) const;
		//unsigned halfword load  
		uint32_t load_uhw(uint32_t addr) const;
		//signed wor load 
		int32_t load_ws(uint32_t addr) const;
		//unsigned word load
		uint32_t load_uw(uint32_t addr) const;
		//store
		void store_byte(uint8_t value,uint32_t addr);
		void store_hw(uint16_t value, uint32_t addr);
		void store_word(uint32_t value, uint32_t addr);
};	


class RegisterFile {
	private:
		std::array<uint32_t, 32> registers_= {0};  // unchanging, reduce 
							   // heap overhead
	public:
		uint32_t read(std::size_t index) const;
		void write(std::size_t index, uint32_t value);
}; 


class CPU {
	private: 
		Memory ram_; 
		RegisterFile regs;
		uint32_t pc_ = 0x80000000; 
	public: 
		
};

#endif // CPU_HPP



