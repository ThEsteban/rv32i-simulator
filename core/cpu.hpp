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


enum class InstructionType{
	R,//register-register arithmetic, opcodes: 0x33
	I,/*immediate arithmetic, opcodes: 0x13(ALU imm), 0x03(loads),
	0x67(jump and link registers), 0x73(system calls), 0x0F(memory fences)
	*/
	S,//stores, opcode: 0x23
	B,//branches, opcode: 0x63
	U,//upper immediates, opcodes: 0x37(load upper imm), 0x17(auipc)
	J,//jump and link, opcode: 0x6F
	UNKNOWN
}; 

struct DecodedInstruction {
	int32_t imm = 0; //immediate value, 
	uint8_t opcode = 0; 
	uint8_t rd = 0; //destination register
	uint8_t funct3 = 0; //sub oberatio identifier 3 bit
	uint8_t rs1= 0; //first source register
	uint8_t rs2 = 0; //second source register
	uint8_t funct7 = 0; //7 bit sub operation identifier
	InstructionType type = InstructionType::UNKNOWN; 
};


class CPU {
	private: 
		Memory ram_; 
		RegisterFile regs;
		uint32_t pc_ = 0x80000000; 
		void clk();
		DecodedInstruction decode(uint32_t instruction);
		static constexpr uint32_t MASK_3bit = 0x07; 
		static constexpr uint32_t MASK_4bit = 0x0F;
		static constexpr uint32_t MASK_5bit = 0x1F;
		static constexpr uint32_t MASK_6bit = 0x3F; 
		static constexpr uint32_t MASK_7bit = 0x7F; 
		static constexpr uint32_t MASK_8bit = 0xFF;
		static constexpr uint32_t MASK_10bit = 0x03FF;
		static constexpr uint32_t MASK_12bit = 0x0FFF;
	public: 
		uint32_t read_pc();
		void reset(); 
};


class ALU {
	private: //addi,  

	public:
};




#endif // CPU_HPP



