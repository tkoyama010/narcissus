#include <cpu.hpp>
#include <string>

namespace narcissus {
    namespace cpu {

        h8_300::h8_300(std::array<std::uint8_t, ROM_SIZE>&& mem)
            : er(), sp(), ccr(), pc(), memory(move(mem))
        {
        }

        auto h8_300::reset_exception() -> void
        {
            // load pc from memory[0] ~ memory[3]
            auto reset_addr = read_immediate(0, 4);

            pc = reset_addr;

            ccr.byte = 0b00000000;
            ccr.interrupt_mask = 1;
        }

        auto h8_300::cycle() -> bool
        {
            std::cout << "pc : 0x" << std::hex << pc << std::endl;
            std::cout << "sp : 0x" << std::hex << sp << std::endl;

            switch (detect_operation()) {
                //                 case ADD_B_IMM: {
                //                     auto rd = memory[pc] & 0x0f;
                //                     auto imm = memory[pc + 1];
                //                     if (!register_write_immediate(rd, imm,
                //                     register_size::BYTE)) {
                //                         return false;
                //                     }

                //                     pc += 2;
                //                     break;
                //                 }

                //                 case ADD_B_R_R:
                //                 {
                //                     auto rs = (memory[pc + 1] & 0xf0) >> 4;
                //                     auto rd = memory[pc + 1] & 0x0f;

                //                     if(!register_write_register(rd, rs,
                //                     register_size::BYTE)){
                //                         return false;
                //                     }

                //                     pc += 2;
                //                     break;
                //                 }

                //                 case ADD_W_IMM:
                //                 {
                //                     auto rd = (memory[pc + 1] & 0xf);
                //                     std::uint16_t imm = std::uint16_t(memory[pc + 2])
                //                     << 8;
                //                     imm |= std::uint16_t(memory[pc + 3]);

                //                     if(!register_write_immediate(rd, imm,
                //                     register_size::WORD))
                //                     {
                //                         return false;
                //                     }

                //                     pc += 4;
                //                     break;
                //                 }

                //                 case ADD_W_R_R:
                //                 {
                //                     auto rs = (memory[pc + 1] & 0xf0) >> 4;
                //                     auto rd = (memory[pc + 1] & 0x0f);

                //                     if(!register_write_register(rd, rs,
                //                     register_size::WORD)){
                //                         return false;
                //                     }

                //                     pc += 2;

                //                     break;
                //                 }

                //                 case ADD_L_IMM:
                //                 {
                //                     std::uint8_t erd = memory[pc + 1] & 0x7;
                //                     auto imm = std::uint32_t(memory[pc + 2]) << 24;
                //                     imm |= std::uint32_t(memory[pc + 3]) << 16;
                //                     imm |= std::uint32_t(memory[pc + 4]) << 8;
                //                     imm |= std::uint32_t(memory[pc + 5]);

                //                     if(!register_write_immediate(std::uint8_t(erd),
                //                     imm, register_size::LONG)){
                //                         return false;
                //                     }

                //                     pc += 6;
                //                     break;
                //                 }

                //                 case ADD_L_R_R:
                //                 {
                //                     auto ers = (memory[pc + 1] & 0x70) >> 8;
                //                     auto erd = memory[pc + 1] & 0x07;

                //                     if(!register_write_register(erd, ers,
                //                     register_size::LONG)){
                //                         return false;
                //                     }

                //                     pc += 2;
                //                     break;
                //                 }

                case operation::ADDS_4:
                {
                    auto erd = memory[pc + 1] & 0x7;

                    auto result = er[erd].er + 4;

                    write_register(erd, result, register_size::LONG);

                    pc += 2;
                    return true;
                }

                case operation::ADD_B_IMM_R:
                {
                    auto rd = memory[pc] & 0xf;
                    auto imm = memory[pc + 1];

                    auto rd_value = read_register(rd, register_size::BYTE);
                    
                    //TODO imm have to cast by integer?
                    auto result = rd_value + imm;

                    write_register(rd, result, register_size::BYTE);

                    update_ccr_sub(rd_value, imm, result, register_size::BYTE);
                    pc += 2;

                    return true;
                }

                case operation::ADD_L_IMM_R:
                {
                    auto erd = memory[pc + 1] & 0x7;
                    auto imm = read_immediate(pc + 2, 4);

                    auto erd_value = er[erd].er;
                    auto result = (std::int32_t)erd_value + (std::int32_t)imm;
                    std::cout << erd_value << ":" 
                        << imm << std::endl;

                    write_register(erd, result, register_size::LONG);

                    update_ccr_sub(erd_value, imm, result, register_size::LONG);
                    pc += 6;
                    return true;
                }

                case operation::SUB_B_R_R: 
                {
                    auto rs = (memory[pc + 1]) >> 4;
                    auto rd = (memory[pc + 1]) & 0xf;

                    auto src_value = read_register(rs, register_size::BYTE);
                    auto dest_value = read_register(rd, register_size::BYTE);
                    auto imm = dest_value - src_value;

                    write_register(rd, imm, register_size::BYTE);

                    update_ccr_sub(src_value, dest_value, imm, register_size::BYTE);

                    pc += 2;
                    break;
                }

                case operation::SUB_W_R_R: 
                {
                    auto rs = memory[pc + 1] >> 0x4;
                    auto rd = memory[pc + 1] & 0xf;

                    auto src_value = read_register(rs, register_size::WORD);
                    auto dest_value = read_register(rd, register_size::WORD);

                    auto result = dest_value - src_value;
        
                    write_register(rd, result, register_size::WORD);
                    update_ccr_sub(src_value, dest_value, result, register_size::WORD);

                    pc += 2;
                    break;
                }

                case operation::SUB_L_R_R:
                {
                    auto ers = (memory[pc + 1] >> 4) & 0x1;
                    auto erd = memory[pc + 1] & 0x7;

                    auto ers_value = er[ers].er;
                    auto erd_value = er[erd].er;

                    auto result = erd_value - ers_value;
                
                    write_register(erd, result, register_size::LONG);

                    update_ccr_sub(ers_value, erd_value, result, register_size::LONG);

                    pc += 2;
                    break;
                }

                case operation::SUB_WITH_SIGN_EXT_1:
                {
                    auto erd = memory[pc + 1] & 0x7;

                    er[erd].er -= 1;

                    pc += 2;
                    break;
                }

                case operation::SUB_WITH_SIGN_EXT_4:
                {
                    auto erd = memory[pc + 1] & 0x7;;

                    er[erd].er -= 4;

                    pc += 2;
                    break;
                }

                case operation::MOV_B_IMM: 
                {
                    auto rd = memory[pc] & 0x0f;
                    auto imm = memory[pc + 1];

                    write_register(rd, imm, register_size::BYTE);

                    update_ccr_mov(imm, register_size::BYTE);
                    pc += 2;
                    break;
                }

                case operation::MOV_B_R_R:
                {
                    auto rs = memory[pc + 1] >> 4;
                    auto rd = memory[pc + 1] & 0xf;

                    auto result = read_register(rs, register_size::BYTE);
                    write_register(rd, result, register_size::BYTE);

                    update_ccr_mov(result, register_size::BYTE);
                    pc += 2;
                    break;
                }

                case operation::MOV_B_R_IND: 
                {
                    auto ers = (memory[pc + 1] & 0x70) >> 4;
                    auto rd = memory[pc + 1] & 0x0f;

                    auto src_reg_value = read_register(ers, register_size::LONG);
                    auto result = memory[src_reg_value];

                    write_register(rd, result, register_size::BYTE);
                    update_ccr_mov(result, register_size::BYTE);
                    pc += 2;

                    break;
                }

                    //                 case operation::MOV_B_R_IND:
                    //                 {
                    //                     auto erd = (memory[pc + 1] & 0x70) >> 4;
                    //                     auto rs = memory[pc + 1] & 0x8;

                    //                     auto src_value = er[rs & 0x7].read(rs,
                    //                     register_size::BYTE);
                    //                     auto dest_value = er[erd & 0x7].read(erd,
                    //                     register_size::LONG);
                    //                     memory[dest_value] = src_value;
                    //
                    //                     update_ccr_mov(src_value, register_size::BYTE);
                    //                     pc += 2;
                    //                     break;
                    //                 }

                case operation::MOV_B_IND_WITH_DIS_16_R: 
                {
                    auto ers = (memory[pc + 1] >> 4) & 0x7;
                    auto rd = memory[pc + 1] & 0xf;

                    auto disp = (std::uint16_t)read_immediate(pc + 2, 2);

                    auto src_value = er[ers].er;
                    auto addr = src_value + (std::int16_t)disp;

                    auto result = memory[addr];

                    write_register(rd, result, register_size::BYTE);

                    update_ccr_mov(result, register_size::BYTE);
                    pc += 4;
                    break;
                }

//                 case operation::MOV_B_IND_WITH_DIS_24_R:
//                 {
//                     auto ers = (memory[pc + 1] & 0x70) >> 4;
//                     auto rd = memory[pc + 3] & 0x7;
//                     auto imm = read_immediate(pc + 5, 3);
//                     auto imm = (std::uint32_t)(memory[pc + 5]) << 16;
//                     imm |= (std::uint32_t)(memory[pc + 6]) << 8;
//                     imm |= (std::uint32_t)memory[pc + 7];

//                     auto ers_value = er[ers].read(ers, register_size::LONG);
//                     auto addr = ers_value + (std::int32_t)imm;

//                     std::cout << std::hex << addr << std::endl;
//                     std::cout << rd << std::endl;
//                     auto result = memory[addr];

//                     if(!register_write_immediate(rd, result, register_size::BYTE))
//                     {
//                         return false;
//                     }

//                     pc += 8;
//                     break;
//                 }

                case operation::MOV_B_R_IND_WITH_DIS_16:
                {

                    auto erd = (memory[pc + 1] >> 4) & 0x7;
                    auto rs = (memory[pc + 1]) & 0xf;
                
                    auto disp = (std::uint16_t)read_immediate(pc + 2, 2);

                    auto src_value = read_register(rs, register_size::BYTE);
                    auto dest_addr = read_register(erd, register_size::LONG);

                    dest_addr += (std::int16_t)disp;
                    memory[dest_addr] = src_value;

                    update_ccr_mov(src_value, register_size::BYTE);
                    pc += 4;
                    break;
                }

                case operation::MOV_B_R_IND_POST_INC:
                {
                    auto ers = (memory[pc + 1] >> 4) & 0x7;
                    auto rd = (memory[pc + 1]) & 0xf;

                    auto result = memory[er[ers].er];
                    write_register(rd, result, register_size::BYTE);

                    update_ccr_mov(result ,register_size::BYTE);

                    er[ers].er += 1;
                    pc += 2;
                    return true;
                }


                case operation::MOV_W_IMM: 
                {
                    auto rd = memory[pc + 1] & 0x7;
                    auto imm = (std::uint16_t)read_immediate(pc + 2, 2);

                    write_register(rd, imm, register_size::WORD);

                    update_ccr_mov(imm, register_size::WORD);
                    pc += 4;
                    break;
                }

                case operation::MOV_W_R_R:
                {
                    auto rs = memory[pc + 1] >> 4;
                    auto rd = memory[pc + 1] & 0xf;

                    auto result = read_register(rs, register_size::WORD);
                    write_register(rd, result, register_size::WORD);

                    write_register(rd, result, register_size::WORD);

                    update_ccr_mov(result,
                            register_size::WORD);

                    pc += 2;
                    return true;
                }

                case operation::MOV_L_IMM: 
                {
                    auto erd = memory[pc + 1] & 0x7;
    
                    auto imm = read_immediate(pc + 2, 4);

                    write_register(erd, imm, register_size::LONG);

                    update_ccr_mov(imm, register_size::LONG);
                    pc += 6;
                    return true;
                }

                case operation::MOV_L_R_R: 
                {
                    auto ers = std::uint32_t(memory[pc + 1] & 0x70) >> 4;
                    auto erd = std::uint32_t(memory[pc + 1] & 0x07);

                    auto src_value = er[ers].er;
                    er[erd].er = src_value;

                    update_ccr_mov(src_value, register_size::LONG);
                    pc += 2;
                    return true;
                }

                case operation::MOV_L_R_IND: 
                {
                    auto erd = (memory[pc + 3] & 0x70) >> 4;
                    auto ers = (memory[pc + 3] & 0x07);

                    auto erd_val = er[erd].er;
                    auto ers_val = er[ers].er;

                    erd_val -= 4;
                    
                    memory[erd_val] = ers_val >> 24;
                    memory[erd_val + 1] = ers_val >> 16;
                    memory[erd_val + 2] = ers_val >> 8;
                    memory[erd_val + 3] = ers_val;

                    er[erd].er = erd_val;

                    update_ccr_mov(ers_val, register_size::LONG);
                    pc += 4;

                    return true;
                }

                case operation::MOV_L_IND_WITH_DIS_24_R: 
                {
                    auto ers = memory[pc + 3] >> 4;
                    auto erd = memory[pc + 5] & 0x7;

                    auto disp = read_immediate(pc + 7, 3);

                    auto addr = (std::int32_t)(er[ers].er) + (std::int32_t)disp;

                    auto dest_value = read_immediate(addr, 4);

                    er[erd].er = dest_value;

                    update_ccr_mov(dest_value, register_size::LONG);
                    pc += 10;
                    return true;
                }

                case operation::MOV_L_R_IND_POST_INC: 
                {
                    auto ers = memory[pc + 3] >> 4;
                    auto erd = memory[pc + 3] & 0x07;

                    auto source_addr = er[ers].er;

                    auto dest_value = read_immediate(source_addr, 4);
                    source_addr += 4;

                    er[erd].er = dest_value;
                    er[ers].er = source_addr;

                    update_ccr_mov(dest_value, register_size::LONG);
                    pc += 4;

                    return true;
                }

                case operation::BEQ: 
                {
                    auto disp = memory[pc + 1];

                    pc += 2;
                    if (ccr.zero != 0x1) {
                        return true;
                    }

                    pc += (std::int8_t)disp;
                    return true;
                }

                case operation::BRA: 
                {
                    auto disp = (std::int8_t)memory[pc + 1];
                    pc += 2;
                    pc += disp;

                    return true;
                }

                case operation::BNE:
                {
                    auto disp = memory[pc + 1];
                    pc += 2;
                    if(ccr.zero == 1) {
                        pc += disp;
                    }
                    return true;
                }

                case operation::CMP_B_IMM:
                {
                    auto rd = memory[pc] & 0xf;
                    auto imm = memory[pc + 1];

                    auto rd_value = read_register(rd, register_size::BYTE);
                    auto result = rd_value - imm;

                    update_ccr_sub(rd_value, imm, result, register_size::BYTE);
                    pc += 2;
                    return true;
                }

                case operation::AND_W:
                {
                    auto rd = memory[pc + 1] & 0xf;
                    auto imm = (std::uint16_t)read_immediate(pc + 2, 2);

                    auto rd_value = read_register(rd, register_size::WORD);
                    auto result = rd_value & imm;
                    write_register(rd, result, register_size::WORD);

                    update_ccr_mov(result, register_size::WORD);
                    pc += 4;
                    return true;
                }

                case operation::AND_B_IMM:
                {
                    auto rd = memory[pc] & 0xf;
                    auto imm = memory[pc + 1];

                    auto rd_value = read_register(rd, register_size::BYTE);
                    auto result = rd_value & imm;

                    write_register(rd, result, register_size::BYTE);

                    pc += 2;
                    update_ccr_mov(result, register_size::BYTE);
                    return true;
                }

                case operation::JSR_ABS: 
                {
                    auto abs = read_immediate(pc + 1, 3);

                    pc += 4;

                    memory[--sp] = (std::uint8_t)(pc & 0x0000ff);
                    memory[--sp] = (std::uint8_t)((pc >> 8) & 0x0000ff);
                    memory[--sp] = (std::uint8_t)((pc >> 16) & 0x0000ff);
                    memory[--sp] = 0x00;

                    pc = abs;
                    return true;
                }

                case operation::EXTS_L: 
                {
                    auto erd = memory[pc + 1] & 0x7;
                    auto val = er[erd].er;
                    uint32_t sign = (uint32_t)(val & 0x00008000);
                    sign |= sign << 1;  // 0x00018000;
                    sign |= sign << 1;  // 0x00038000;
                    sign |= sign << 2;  // 0x000f8000;
                    sign |= sign << 4;  // 0x00ff8000;
                    sign |= sign << 8;  // 0xffff8000;

                    auto result = (val & 0xffff) | sign;
                    er[erd].er = result; //(val & 0x00008fff) | sign;

                    update_ccr_mov(result, register_size::LONG);
                    pc += 2;
                    return true;
                }

                case operation::SHLL_L: 
                {
                    auto erd = memory[pc + 1] & 0x7;
                    er[erd].er = er[erd].er << 1;

                    update_ccr_shll(er[erd].er, register_size::LONG);
                    pc += 2;
                    return true;
                }

                case operation::RTS: 
                {
                    // memory[sp]: is reserved
                    memory[sp++];
                    auto return_addr = read_immediate(sp, 3);
                    sp += 3;

                    pc = return_addr;

                    return true;
                }

                case operation::INVALID:
                          std::cout << "INVALID opecode: " << std::hex << "0x" << std::flush;
                          std::cout << std::setw(2) << std::setfill('0')
                              << (std::uint16_t)(memory[pc]) << std::flush;
                          std::cout << std::setw(2) << std::setfill('0')
                              << (std::uint16_t)(memory[pc + 1]) << std::endl;

                          std::cout << "pc             : 0x" << std::setw(8)
                              << std::setfill('0') << (std::uint32_t)pc << std::endl;

                          return false;

                default:
                          std::cout << "implement yet" << std::endl;
                          return false;
            }

            return true;
        }

        auto h8_300::detect_operation() -> operation
        {
            std::uint8_t op = memory[pc];

            auto ah = op >> 4;
            auto al = op & 0x0f;

            auto op2 = memory[pc + 1];
            auto bh = op2 >> 4;
            auto bl = op2 & 0x0f;

            auto op3 = memory[pc + 2];
            auto ch = op3 >> 4;
            auto cl = op3 & 0x0f;

            switch (ah) {
                case 0:
                    switch (al) {
                        case 1:
                            switch (bh) {
                                case 0:
                                    switch (bl) {
                                        case 0:
                                            switch (ch) {
                                                case 6:
                                                    switch (cl) {
                                                        case 0xd: {
                                                                      auto dh =
                                                                          memory[pc + 3] & 0x80;
                                                                      if (dh != 0x80) {
                                                                          return operation::
                                                                              MOV_L_R_IND_POST_INC;
                                                                      }
                                                                      return operation::
                                                                          MOV_L_R_IND;
                                                                  }
                                                        default:
                                                                  return operation::INVALID;
                                                    }
                                                case 7:
                                                    switch (cl) {
                                                        case 8: {
                                                                    auto t =
                                                                        memory[pc + 5] >> 4;
                                                                    switch (t) {
                                                                        case 0x2:
                                                                            return operation::
                                                                                MOV_L_IND_WITH_DIS_24_R;
                                                                        default:
                                                                            return operation::INVALID;
                                                                    }
                                                                }
                                                        default:
                                                                return operation::INVALID;
                                                    }
                                                default:
                                                    return operation::INVALID;
                                            }
                                        default:
                                            return operation::INVALID;
                                    }
                                default:
                                    return operation::INVALID;
                            }
                        case 8:
                            //                             return operation::ADD_B_R_R;
                            return operation::INVALID;
                        case 9:
                            //                             return operation::ADD_W_R_R;
                            return operation::INVALID;
                        case 0xa:
                            switch (bh) {
                                case 0x8:
                                case 0x9:
                                case 0xa:
                                case 0xb:
                                case 0xc:
                                case 0xd:
                                case 0xe:
                                case 0xf:
                                    //                                     return
                                    //                                     operation::ADD_L_R_R;

                                default:
                                    return operation::INVALID;
                            }

                        case 0xb:
                            switch (bh) {
                                case 0x0:
//                                     return operation::ADDS_1;
                                case 0x8:
//                                     return operation::ADDS_2;
                                case 0x9:
                                    return operation::ADDS_4;

                                default:
                                    return operation::INVALID;
                            }

                        case 0xc:
                            return operation::MOV_B_R_R;

                        case 0xd:
                            return operation::MOV_W_R_R;

                        case 0xf:
                            return operation::MOV_L_R_R;

                        default:
                            return operation::INVALID;
                    }
                case 1:
                    switch (al) {
                        case 0:
                            switch (bh) {
                                case 3:
                                    return operation::SHLL_L;
                                default:
                                    return operation::INVALID;
                            }
                        case 7:
                            switch (bh) {
                                case 0xf:
                                    return operation::EXTS_L;

                                default:
                                    return operation::INVALID;
                            }
                        case 8:
                            return operation::SUB_B_R_R;

                        case 9:
                            return operation::SUB_W_R_R;

                        case 0xa:
                            if((bh >> 3) & 0x1){
                                return operation::SUB_L_R_R;
                            }
                            return operation::INVALID;

                        case 0xb:
                            switch (bh) {
                                case 0:
                                    return operation::SUB_WITH_SIGN_EXT_1;
                                case 8:
//                                     return operation::SUB_WITH_SIGN_EXT_2;
                                    return operation::INVALID;
                                case 9:
                                    return operation::SUB_WITH_SIGN_EXT_4;

                                default:
                                    return operation::INVALID;
                            }

                        default:
                            return operation::INVALID;
                    }

                case 4:
                    switch (al) {
                        case 0:
                            return operation::BRA;

                        case 6:
                            return operation::BNE;

                        case 7:
                            return operation::BEQ;

                        default:
                            return operation::INVALID;
                    }

                case 5:
                    switch (al) {
                        case 0x4:
                            switch (bh) {
                                case 0x7:
                                    switch (bl) {
                                        case 0x0:
                                            return operation::RTS;

                                        default:
                                            return operation::INVALID;
                                    }
                                default:
                                    return operation::INVALID;
                            }

                        case 0xe:
                            return operation::JSR_ABS;

                        default:
                            return operation::INVALID;
                    }

                case 6:
                    switch (al) {
                        case 0x8:
                            return operation::MOV_B_R_IND;

                        case 0xc:
                            return operation::MOV_B_R_IND_POST_INC;

                        case 0xe:
                            {
                                auto t = memory[pc + 1] >> 7;
                                switch (t) {
                                    case 0:
                                        return operation::MOV_B_IND_WITH_DIS_16_R;

                                    case 1:
                                        return operation::MOV_B_R_IND_WITH_DIS_16;

                                    default:
                                        return operation::INVALID;
                                }
                            }
                        default:
                            return operation::INVALID;
                    }

                case 7:
                    switch (al) {
                        case 8:
                        {
                            auto t = bh >> 3;
                            if (t == 0x0) {
                                switch (bl) {
                                    case 0:
                                        switch (ch) {
                                            case 6:
                                                switch (cl) {
                                                    case 0xa:
                                                    {
                                                        auto dh = memory[pc + 3] >> 4;
                                                        auto dl = memory[pc + 3] & 0xf;
                                                        auto eh = memory[pc + 4] >> 4;
                                                        auto el = memory[pc + 4] & 0xf;
                                                        if(dh == 0x2 && eh == 0 && el == 0){
//                                                             return operation::MOV_B_IND_WITH_DIS_24_R;
                                                        }
                                                        return operation::INVALID;
                                                    }
                                                    default:
                                                        return operation::INVALID;
                                                }
                                            default:
                                                return operation::INVALID;
                                        }

                                    default:
                                        return operation::INVALID;
                                }
                            }
                            return operation::INVALID;
                        }
                        case 9:
                            switch (bh) {
                                case 1:
                                    return operation::INVALID;
                                case 0:
                                    return operation::MOV_W_IMM;
                                case 6:
                                    return operation::AND_W;
                                default:
                                    return operation::INVALID;
                            }


                        case 0xa:
                            switch (bh) {
                                case 0:
                                    return operation::MOV_L_IMM;
                                case 1:
                                    return operation::ADD_L_IMM_R;
                                default:
                                    return operation::INVALID;
                            }
                        default:
                            return operation::INVALID;
                    }

                case 8:
                    return operation::ADD_B_IMM_R;

                case 0xa:
                    return operation::CMP_B_IMM;

                case 0xe:
                    return operation::AND_B_IMM;

                case 0xf:
                    return operation::MOV_B_IMM;

                default:
                    return operation::INVALID;
            }
        }

        auto h8_300::update_ccr_sub(std::uint32_t value_0,
                std::uint32_t value_1,
                std::uint64_t result,
                register_size r_size)
            -> void
        {
            auto size = (std::uint32_t)r_size;
            auto carry_shift_size = size + 1;
            int sign_0 = value_0 >> size;
            int sign_1 = value_1 >> size;
            int sign_result = result >> size;

            ccr.carry = (result >> carry_shift_size) & 0x1;

            if (result == 0) {
                ccr.zero = 1;
            } else {
                ccr.zero = 0;
            }

            ccr.negative = (sign_result >> size) & 0x1;
            ccr.over_flow = sign_0 != sign_1 && sign_1 != sign_result;
        }

        auto h8_300::update_ccr_mov(uint64_t value, register_size size)
            -> void
        {
            ccr.over_flow = 0;

            if (value == 0) {
                ccr.zero = 1;
            } else {
                ccr.zero = 0;
            }

            ccr.negative = (value >> (std::uint32_t)size) & 0x1;
        }

        auto h8_300::update_ccr_shll(uint64_t value, register_size size)
            -> void
        {
            update_ccr_mov(value, size);
            ccr.carry = 0x00010000 & value;
        }

        auto h8_300::read_register_fields(std::uint32_t address, 
                value_place place, bool is_32bit) -> std::uint8_t
        {
            auto reg = memory[address];

            switch (place) {
                case value_place::high:
                    reg = (reg & 0xf0) >> 4;
                    break;

                case value_place::low:
                    reg = reg & 0x0f;
                    break;
            }

            if(is_32bit) {
                return reg & 0x7;
            }
            else {
                return reg;
            }
        }

        auto h8_300::read_immediate(std::uint32_t address, 
                std::uint8_t number_of_byte) -> std::uint32_t
        {
            auto imm = (std::uint32_t)memory[address] << (8 * (number_of_byte - 1));
            for(auto i = 1; i < number_of_byte; ++i)
            {
                imm |= (std::uint32_t)memory[address + i] << ((number_of_byte - i - 1) * 8);
            }

            return imm;
        }

        auto h8_300::write_immediate(std::uint32_t base, 
                std::uint8_t number_of_byte, std::uint32_t immediate) -> void
        {
            for(auto i = 0; i < number_of_byte; ++i)
            {
                memory[base + i] = immediate >> (number_of_byte - i - 1) * 8;
            }
        }

        auto h8_300::read_register(std::uint8_t source, register_size size) 
            -> std::uint32_t
        {
            auto reg = er[source & 0x7];
            switch (size) {
                case register_size::BYTE:
                    if((source & 0x8) != 0x8){
                        return reg.h;
                    }
                    else {
                        return reg.l;
                    }
                case register_size::WORD:
                    if((source & 0x8) != 0x8) {
                        return reg.r;
                    }
                    else {
                        return reg.e;
                    }
                case register_size::LONG:
                    return reg.er;
            }
        }

        auto h8_300::write_register(std::uint8_t destination, 
                        std::uint32_t value, register_size size) -> void
        {
            auto &reg = er[destination & 0x7];
            switch (size) {
                case register_size::BYTE:
                    if((destination & 0x8) != 0x8) {
                        reg.h = std::uint8_t(value);
                    }
                    else {
                        reg.l = std::uint8_t(value);
                    }
                    break;
                case register_size::WORD:
                    if((destination & 0x8) != 0x8){
                        reg.r = std::uint16_t(value);
                    }
                    else {
                        reg.e = std::uint16_t(value);
                    }
                    break;
                case register_size::LONG:
                    reg.er = value;
                    break;
            }
        }


    }  // namespace cpu
}  // namespace narcissus
