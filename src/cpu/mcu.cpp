#include <mcu.hpp>
#include <exception>
#include <cassert>

#include <iostream>

namespace narcissus {
    namespace cpu {

        mcu::mcu(std::array<std::uint8_t, ROM_SIZE>&& init_rom,
                std::shared_ptr<std::condition_variable> c_variable_ptr,
                std::weak_ptr<h8_300> cpu,
                std::shared_ptr<bool> is_sleep) 
            : rom(move(init_rom)), ram(),
            sci_1(std::make_shared<sci::sci>(c_variable_ptr, is_sleep))
        {}

        auto mcu::operator[] (std::uint32_t address)
            -> std::uint8_t&
        {
//             std::cout << address << std::endl;
            if(address >= ROM_BASE_ADDR && address < ROM_END_ADDR) {
                return rom[address];
            }

            if(address >= RAM_BASE_ADDR && address < RAM_END_ADDR) {
                return ram[address - RAM_BASE_ADDR];
            }

            //add MMI/O
            //TODO
            //consider about sci0 and sci2.
            if(address >= SCI1_BASE_ADDR && address < SCI2_BASE_ADDR)
            {
                return (*sci_1)[address];
            }

            std::cout << "memory access error: 0x"<< address << std::endl;
            assert(false);
        }

    } // namespace cpu
} // namespace narcissus
