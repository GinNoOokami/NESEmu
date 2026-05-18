#ifndef NESEMU_APP_HPP
#define NESEMU_APP_HPP

class App {
public:
    virtual ~App() = default;

    virtual void initialize() = 0;
    virtual void run() = 0;
    virtual void shutdown() = 0;
};

#endif //NESEMU_APP_HPP
