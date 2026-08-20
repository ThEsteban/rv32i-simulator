#include "cpu.hpp"

std::size_t Memory::translate_address(uint32_t raddress) const {
	assert(raddress >= BASE_OFFSET && "real address too low") ; 
	std::size_t vaddress = raddress - BASE_OFFSET; 
	assert(vaddress < RAM_SIZE); 
	return vaddress; 
}

Memory::Memory(): memory_(RAM_SIZE,0)  {}

int32_t Memory::load_byte(uint32_t addr) const{//signed 
	return static_cast<int8_t>(memory_[translate_address(addr)]);
}

uint32_t Memory::load_ubyte(uint32_t addr) const { // unsigned ld
	return memory_[translate_address(addr)];
}

int32_t Memory::load_hws(uint32_t addr) const { //little endian
	std::size_t offset = translate_address(addr); //save time;  
	assert(offset + 1 < RAM_SIZE && "bug in offset");
	uint16_t raw = memory_[offset+1] << 8;                   
	raw = raw | memory_[offset];
	return static_cast<int16_t>(raw); 
}

uint32_t Memory::load_uhw(uint32_t addr) const {
	std::size_t offset = translate_address(addr); //save time;  
	assert(offset + 1 < RAM_SIZE && "loaduhw out of bounds");  
	uint16_t raw = memory_[offset+1] << 8; 
	raw = raw | memory_[offset]; 
	return raw; 
}

int32_t Memory::load_ws(uint32_t addr) const {
	std::size_t offset = translate_address(addr); 
	assert(offset+3 < RAM_SIZE);
	uint32_t raw = static_cast<uint32_t>(memory_[offset+3])<<24 |			     (static_cast<uint32_t>(memory_[offset+2]) << 16 )|
	(static_cast<uint32_t>(memory_[offset+1]) << 8 )|
	(static_cast<uint32_t>(memory_[offset])); 
	return static_cast<int32_t>(raw); 
}

uint32_t Memory::load_uw(uint32_t addr) const{
	std::size_t offset = translate_address(addr); 
	assert(offset+3 < RAM_SIZE); 
	uint32_t raw = static_cast<uint32_t>(memory_[offset+3])<<24 |                        (static_cast<uint32_t>(memory_[offset+2]) << 16 )|
	(static_cast<uint32_t>(memory_[offset+1]) << 8 )|
	(static_cast<uint32_t>(memory_[offset]));
	return raw; 
}

void Memory::store_byte(uint8_t value,uint32_t addr){//store byte unsigned
	memory_[translate_address(addr)] = value;  
}

void Memory::store_hw(uint16_t value, uint32_t addr) {
	std::size_t offset = translate_address(addr); 
	assert(offset + 1 < RAM_SIZE && "sthw out of bounds"); 
	memory_[offset] = static_cast<uint8_t>(value) ; 
	memory_[offset +1] = static_cast<uint8_t>(value >> 8); 
}

void Memory::store_word(uint32_t value, uint32_t addr) {
	std::size_t offset = translate_address(addr); 
	assert(offset + 3 < RAM_SIZE && "stw out of bounds"); 
	memory_[offset] = static_cast<uint8_t>(value); 
	memory_[offset+1] = static_cast<uint8_t>(value >> 8);
	memory_[offset+2] = static_cast<uint8_t>(value >> 16);
	memory_[offset+3] = static_cast<uint8_t>(value >> 24);
}

uint32_t RegisterFile::read(std::size_t index) const {
	assert(index < registers_.size());
	return registers_[index];
}  

void RegisterFile::write(std::size_t index, uint32_t value) {
	assert(index < registers_.size()); 
	if (index == 0) return; 
	registers_[index] = value; 
}

DecodedInstruction CPU::decode(uint32_t instruction){
	DecodedInstruction decInstruction; 
	decInstruction.opcode = instruction & 0x7F ; //isiolate bottom 7 bits
	switch(decInstruction.opcode){// switch to fetch type of instruction 
	case 0x33: 
		decInstruction.type = InstructionType::R ; 
		break; 
	case 0x13:
	case 0x03:
	case 0x67:
	case 0x73:
	case 0x0F:
		decInstruction.type = InstructionType::I ; 
		break; 
	case 0x23:
		decInstruction.type = InstructionType::S ; 
		break;
	case 0x63:
		decInstruction.type = InstructionType::B; 
		break;
	case 0x37:
	case 0x17:
		decInstruction.type = InstructionType::U ; 
		break; 
	case 0x6F:
		decInstruction.type = InstructionType::J ;
		break;
	default:
		decInstruction.type = InstructionType::UNKNOWN; 
	}
	switch(decInstruction.type){ // fetch necessary bits for each op
	case InstructionType::R :{
		decInstruction.rd = (instruction>>7) & MASK_5bit ; 
		decInstruction.funct3 = (instruction >>12) & MASK_3bit; 
		decInstruction.rs1 = (instruction >>15) & MASK_5bit ; 
		decInstruction.rs2 = (instruction >> 20 ) & MASK_5bit;
		decInstruction.funct7 = (instruction >>25) & MASK_7bit; 
		break; 
	}
	case InstructionType::I : {
		decInstruction.imm = static_cast<int32_t>(((instruction >>20) & MASK_12bit) <<20) >> 20;
		decInstruction.rd = (instruction >> 7) & MASK_5bit;
		decInstruction.funct3 = (instruction >> 12 ) & MASK_3bit; 
		decInstruction.rs1 = (instruction >>15) & MASK_5bit; 
		break; 
	}
	case InstructionType::S : {
		uint32_t raw_imms = ((instruction >> 7 ) & MASK_5bit )|//process to get 12 bit immediate 
							(((instruction >> 25 ) & MASK_7bit) << 5);
		decInstruction.imm = static_cast<int32_t>(raw_imms << 20) >>20; 
		decInstruction.funct3 = (instruction >> 12) & MASK_3bit; 
		decInstruction.rs1 = (instruction >> 15) & MASK_5bit; 
		decInstruction.rs2 = (instruction >>20) & MASK_5bit; 
		break; 
	}
	case InstructionType::B :{
		/*decInstruction.imm = (instruction <<4 ) & 0x800 ; //isolate instr[7] and put it in imm[11]
		uint8_t immb2 = (instruction >> 7) & 0x1E ; //isolate lowest 4 bits for imm b type
		uint16_t immb3 = (instruction >>20) & 0x07E0; //isolate imm[10:5] 
		uint16_t immb4 = (instruction >>19) & 0x1000; //isolate imm[12]		sign extend
		decInstruction.imm = ((decInstruction.imm | immb2 | immb3 | immb4) <<19) >>19; //combine into 12bitimm
		*/
		uint32_t raw_imm = ((instruction >> 19) & 0x1000) | // imm[12]
                       ((instruction << 4)  & 0x0800) | // imm[11]
                       ((instruction >> 20) & 0x07E0) | // imm[10:5]
                       ((instruction >> 7)  & 0x001E);  // imm[4:1]
        decInstruction.imm = static_cast<int32_t>(raw_imm <<19)>>19; 
		decInstruction.funct3 = (instruction >> 12) & MASK_3bit; 
		decInstruction.rs1 = (instruction >> 15) & MASK_5bit; 
		decInstruction.rs2 = (instruction >> 20) & MASK_5bit; 
		break; 
	}
	case InstructionType::U :{
		decInstruction.rd = (instruction >> 7) & MASK_5bit; 
		decInstruction.imm = static_cast<int32_t>(instruction & 0xFFFFF000); 
		break;
	}
	case InstructionType::J : {
		decInstruction.rd = (instruction >> 7) & MASK_5bit; 
		uint32_t raw_immj= ((instruction >>20) & 0x07FE) | //imm[10:1]
							((instruction>>11) & 0x100000) | //imm[20]
							((instruction >>9 ) & 0x800) | // imm[11]
							( instruction  & 0x000FF000); // imm[19:12]
		decInstruction.imm = static_cast<int32_t>(raw_immj << 11) >> 11; 
		break;
	}
	default:
		break;  //type is unknown, handle illegal instruction in execution phase
	}
	return decInstruction ; 
}

uint32_t CPU::execute_branch(DecodedInstruction instruction, uint32_t current_pc){
	uint32_t src1 = instruction.rs1; 
	uint32_t src2 = instruction.rs2; 
	bool takebr = false; 
	switch(instruction.funct3){
		case 0b000: takebr = (src1 == src2); break; //BEQ, zero flag 
		case 0b001: takebr = (src1 != src2); break; //BNE, zero flag
		case 0b100: takebr = ((int32_t)src1 < (int32_t)src2); break; // BLT, 
        case 0b101: takebr = ((int32_t)src1 >= (int32_t)src2); break; // BGE
        case 0b110: takebr = (src1 < src2); break;        // BLTU
        case 0b111: takebr = (src1 >= src2); break;       // BGEU
        default:
            //handle illegal instruction exception here (for now, just assert or return pc+4)
            assert(false && "Illegal funct3 for B-type instruction");
            return current_pc + 4;
    }
	if(takebr){
		return current_pc + (instruction.imm * 2); 
	}else{
		return current_pc + 4; 
	}

}

uint32_t CPU::execute(DecodedInstruction instruction, uint32_t current_pc){

}

uint32_t ALU::compute(uint32_t a , uint32_t b, ALUop operation){
	switch(operation){
		case(ALUop::ADD):{
			break; 
		}
		case(ALUop::SUB):{
			break;
		}
		case(ALUop::AND):{
			break;
		}
		case(ALUop::OR):{
			break;
		}
		case(ALUop::XOR):{
			break;
		}
		case(ALUop::SLL):{
			break;
		}
		case(ALUop::SRL):{
			break;
		}
		case(ALUop::SRA):{
			break;
		}
		case(ALUop::SLT):{
			break;
		}
		case(ALUop::SLTU):{
			break;
		}
		default: /*illegal op*/ break; 
	}
}



void CPU::clk(){
	uint32_t instruction = ram_.load_uw(pc_); //fetch
	DecodedInstruction decInstruction = decode(instruction);//decode
	uint32_t current_pc =  pc_; //save current pc for JAL/JALR
	if(decInstruction.type== InstructionType::B){
		CPU::execute_branch(decInstruction, current_pc);
	}else{
		uint32_t next_pc = CPU::execute(decInstruction, current_pc); 
		pc_ = next_pc; 
	}
}


uint32_t CPU::read_pc(){
	return pc_; 
}