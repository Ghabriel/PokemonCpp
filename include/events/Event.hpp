#ifndef EVENT_HPP
#define EVENT_HPP

class Event {
 public:
    virtual ~Event() = default;

    void onStart() {
        onStartImpl();
    }

    bool tick() {
        return tickImpl();
    }

 private:
    virtual void onStartImpl() = 0;
    virtual bool tickImpl() = 0;
};

#endif
