#pragma once

#include <stdint.h>

class Linker
{
    friend class DataSync;
    friend class Position;
protected:
    Linker() = default;
    virtual ~Linker() = default;

    void set_parent(Linker* p);
    void set_next(Linker* p);
    void set_prev(Linker* p);

    Linker* parent() const;
    Linker* next() const;
    Linker* prev() const;

private:
    Linker* next_ = nullptr;
    Linker* prev_ = nullptr;
    Linker* parent_ = nullptr;
};

class Position
{
    friend class DataSync;
protected:
    Position() : offset_(0) {}
    virtual ~Position() {}

    void set_offset(size_t offset) { offset_ = offset; }
    void change_offset(int offset) { if (offset < 0) offset_ -= (-1 *offset); else offset_ += offset; }
    size_t get_offset() { return offset_; }
    size_t get_pos();
private:
    size_t offset_;
};

class DataSync
{
public:
    DataSync() {}
    virtual ~DataSync() {}

    static int calc_move_distance(const class XData* new_node, const class XData* old_node);
    static int sync(class XData* data, int offset);
    static int update_type(class XData* data, int8_t type);
    static int refresh_offset(XData* data);
};

