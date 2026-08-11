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
		std::size_t translate_address(uint32_t raddress) const {
			assert(raddress >= BASE_OFFSET && "real address too low") ; 
			std::size_t vaddress = raddress - BASE_OFFSET; 
			assert(vaddress < RAM_SIZE); 
			return vaddress; 
		}
			
	public:
		Memory(): memory_(RAM_SIZE,0)  {}
		//load word, halfword, byte, signed/unsigned
		//byte, load whatever's at given memory address into destination
		int32_t load_byte(uint32_t addr) const{//signed 
 		       return static_cast<int8_t>(memory_[translate_address(addr)]);
		}
		uint32_t load_ubyte(uint32_t addr) const { // unsigned ld
			return memory_[translate_address(addr)];
		}

		//load halfword signed 
		int32_t load_hws(uint32_t addr) const { //little endian
			std::size_t offset = translate_address(addr); //save time;  
                        assert(offset + 1 < RAM_SIZE && "bug in offset");
                        uint16_t raw = memory_[offset+1] << 8;                   
                        raw = raw | memory_[offset];
			return static_cast<int16_t>(raw); 
		}
		//unsigned halfword load  
		uint32_t load_uhw(uint32_t addr) const {
			std::size_t offset = translate_address(addr); //save time;  
			assert(offset + 1 < RAM_SIZE && "loaduhw out of bounds");  
			uint16_t raw = memory_[offset+1] << 8; 
			raw = raw | memory_[offset]; 
			return raw; 
		}
		//signed wor load 
		int32_t load_ws(uint32_t addr) const {
			std::size_t offset = translate_address(addr); 
			assert(offset+3 < RAM_SIZE);
		       	uint32_t raw = static_cast<uint32_t>(memory_[offset+3])<<24 |			     (static_cast<uint32_t>(memory_[offset+2]) << 16 )|
			(static_cast<uint32_t>(memory_[offset+1]) << 8 )|
			(static_cast<uint32_t>(memory_[offset])); 
			return static_cast<int32_t>(raw); 
		}
		//unsigned word load
		uint32_t load_uw(uint32_t addr) const{
			std::size_t offset = translate_address(addr); 
			assert(offset+3 < RAM_SIZE); 
			uint32_t raw = static_cast<uint32_t>(memory_[offset+3])<<24 |                        (static_cast<uint32_t>(memory_[offset+2]) << 16 )|
                        (static_cast<uint32_t>(memory_[offset+1]) << 8 )|
                        (static_cast<uint32_t>(memory_[offset]));
			return raw; 
		}
		//store
		void store_byte(uint8_t value,uint32_t addr){//store byte unsigned
			memory_[translate_address(addr)] = value;  
		}
		void store_hw(uint16_t value, uint32_t addr) {
			std::size_t offset = translate_address(addr); 
			assert(offset + 1 < RAM_SIZE && "sthw out of bounds"); 
			memory_[offset] = static_cast<uint8_t>(value) ; 
			memory_[offset +1] = static_cast<uint8_t>(value >> 8); 
		}
		void store_word(uint32_t value, uint32_t addr) {
			std::size_t offset = translate_address(addr); 
			assert(offset + 3 < RAM_SIZE && "stw out of bounds"); 
			memory_[offset] = static_cast<uint8_t>(value); 
			memory_[offset+1] = static_cast<uint8_t>(value >> 8);
			memory_[offset+2] = static_cast<uint8_t>(value >> 16);
			memory_[offset+3] = static_cast<uint8_t>(value >> 24);
		}
};	


class RegisterFile {
	private:
		std::array<uint32_t, 32> registers_= {0};  // unchanging, reduce 
							   // heap overhead
	public:
		uint32_t read(std::size_t index) const {
			assert(index < registers_.size());
			return registers_[index];
		}  
		void write(std::size_t index, uint32_t value) {
			assert(index < registers_.size()); 
			if (index == 0) return; 
			registers_[index] = value; 
		}
}; 


class CPU {
	private: 
		Memory ram_; 
		RegisterFile regs;
		uint32_t pc_ = 0x80000000; 
	public: 
		
};




