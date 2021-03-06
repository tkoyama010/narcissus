#include <mcu.hpp>

#include <cassert>
#include <iostream>

namespace narcissus {
    namespace h8_3069f {

        mcu::mcu(std::array<std::uint8_t, (std::uint32_t)mem_info::rom_size>&& init_rom,
                std::shared_ptr<std::condition_variable> c_variable_ptr,
                std::shared_ptr<bool> is_sleep, std::shared_ptr<std::mutex> m) 
            : rom(move(init_rom)), ram(),
            sci_1(std::make_shared<sci>(c_variable_ptr, is_sleep, m))
        {}

        auto mcu::before_run(std::shared_ptr<cpu> c) -> void
        {
            sci_1->before_run(c);
        }

        auto mcu::operator[] (std::uint32_t address)
            -> std::uint8_t&
        {
//             std::cout << address << std::endl;
            if(address >= (std::uint32_t)mem_info::rom_base_addr 
                    && address < (std::uint32_t)mem_info::rom_end_addr) {
                return rom[address];
            }

            if(address >= (std::uint32_t)mem_info::ram_base_addr 
                    && address < (std::uint32_t)mem_info::ram_end_addr) {
                return ram[address - (std::uint32_t)mem_info::ram_base_addr];
            }

            //add MMI/O
            //TODO
            //consider about sci0 and sci2.
            
            //for sci1
            if(address >= (std::uint32_t)mem_info::sci1_base_addr 
                    && address < (std::uint32_t)mem_info::sci2_base_addr)
            {
                return (*sci_1)[address];
            }

            std::clog << "memory access error: 0x" << std::hex << address << std::endl;
            assert(false);
        }

    } // namespace h8_3069f
} // namespace narcissus
