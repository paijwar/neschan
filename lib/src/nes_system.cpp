#include "stdafx.h"

#include "nes_cpu.h"
#include "nes_system.h"
#include "nes_ppu.h"

using namespace std;

nes_system::nes_system()
{
    _ram = make_unique<nes_memory>();
    _cpu = make_unique<nes_cpu>();
    _ppu = make_unique<nes_ppu>();

    _components.push_back(_ram.get());
    _components.push_back(_cpu.get());
    _components.push_back(_ppu.get());

    _stop_requested = false;
}
                         
nes_system::~nes_system() {}

void nes_system::power_on()
{
    for (auto comp : _components)
        comp->power_on(this);
}

void nes_system::reset()
{
    for (auto comp : _components)
        comp->reset();
}

void nes_system::run_program(vector<uint8_t> &&program, uint16_t addr)
{
    power_on();

    _ram->set_bytes(addr, program.data(), program.size());
    _cpu->PC() = addr;

    main_loop();
}

void nes_system::run_rom(const char *rom_path)
{
    power_on();

    auto mapper = nes_rom_loader::load_from(rom_path);
    _ram->load_mapper(mapper);
    _cpu->PC() = mapper->get_code_addr();

    main_loop();
}

void nes_system::main_loop()
{
    while (!_stop_requested)
    {
        step(1);

        // @TODO - add proper time synchronization/delay
    }
}

void nes_system::step(nes_cycle_t count)
{
    _master_cycle += count;

    for (auto comp : _components)
        comp->step_to(_master_cycle);
}