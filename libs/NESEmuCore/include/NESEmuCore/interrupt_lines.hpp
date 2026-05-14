#ifndef NESEMU_INTERRUPT_LINES_HPP
#define NESEMU_INTERRUPT_LINES_HPP

namespace NESEmu {
struct InterruptLines {
    bool nmiActive = false;
    bool irqActive = false;

    void reset()
    {
        nmiActive = false;
        irqActive = false;
    }
};
}

#endif //NESEMU_INTERRUPT_LINES_HPP
