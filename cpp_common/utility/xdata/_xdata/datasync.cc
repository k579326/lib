
#include "datasync.h"

#include <assert.h>

#include <queue>
#include <stack>


#include "xdata.h"
#include <dataencoder.h>
#include <datasource.h>


size_t Position::get_pos()
{
    size_t result = 0;
    std::stack<Linker*> path;
    Linker* _start = dynamic_cast<Linker*>(this);
    while (_start->parent())
    {
        result += dynamic_cast<Position*>(_start)->get_offset();
        _start = _start->parent();
    }
    return result;
}

int DataSync::refresh_offset(XData* data)
{
    XData* node = nullptr;

    if (data->prev())
    {
        node = dynamic_cast<XData*>(data->prev());
        data->set_offset(node->get_offset() + DataEncoder::Size(node));
    }
    else if (data->parent())
    {
        node = dynamic_cast<XData*>(data->parent());
        if (node->type() == ElmType::Pair)
        {
            data->set_offset(DataEncoder::Size(node) - DataEncoder::Size(data));
        }
        else
        {
            data->set_offset(5);
        }
    }
    else
    {
        data->set_offset(0);
    }
    return 0;
}

int DataSync::sync(XData* data, int offset)
{
    std::shared_ptr<IDataSource> source = nullptr;
    size_t pos_of_data = 0, pos_of_next = 0;
    size_t size_of_data = DataEncoder::Size(data);
    std::queue<XData*> path;
    std::stack<XData*> path2;
    XData* _start = data;
    XData* _next_of_data = nullptr;
    XData* _free_pointer = data;

    refresh_offset(data);

    while (_start)
    {
        path.push(_start);
        path2.push(_start);
        _start = dynamic_cast<XData*>(_start->parent());
    }

    source = path2.top()->source();

    _next_of_data = const_cast<XData*>(_free_pointer->next_sibling());
    while (!_next_of_data)
    {
        _free_pointer = dynamic_cast<XData*>(_free_pointer->parent());
        if (!_free_pointer) break;
        _next_of_data = const_cast<XData*>(_free_pointer->next_sibling());
    }

    if (_next_of_data)
    {
        pos_of_next = _next_of_data->get_pos();
    }

    // adjust offset
    while (!path.empty())
    {
        _start = path.front();
        path.pop();

        for (auto it = _start->next_sibling(); it != nullptr; it = it->next_sibling())
        {
            const_cast<XData*>(it)->change_offset(offset);
        }
    }

    if (!source) return -1;

    // move data
    pos_of_data = data->get_pos();

    int size_changed = offset;
    if (!source->Writable(size_changed > 0 ? (size_t)size_changed : 0))
    {
        return -1;
    }

    // move data
    if (_next_of_data)
    {
        source->MoveData((int)pos_of_next + offset, pos_of_next, -1);
    }

    // write this node
    DataEncoder::XDataStream stream;
    stream.first = new char[size_of_data];
    stream.second = 0;
    DataEncoder::encode(data, stream);
    assert(size_of_data == stream.second);
    source->Write(pos_of_data, 0, stream.first, stream.second);
    delete[] stream.first;

    // change size of the node in path
    size_t _stream_pos = 0;
    while (path2.size() > 1)
    {
        auto node = path2.top();
        path2.pop();

        _stream_pos = node->get_pos();
        int32_t org_len = source->ReadAsInt32(_stream_pos, 4);
        int32_t new_len = org_len + offset;
        source->Write(_stream_pos, 0, &new_len, 4);
    }

    // update file header

    return 0;
}

int DataSync::calc_move_distance(const class XData* new_node, const class XData* old_node)
{
    if (!new_node && !old_node)
    {
        return 0;
    }
    if (!new_node && old_node)
    {
        return -(int)DataEncoder::Size(old_node);
    }
    if (new_node && !old_node)
    {
        return (int)DataEncoder::Size(new_node);
    }
    return (int)DataEncoder::Size(new_node) - (int)DataEncoder::Size(old_node);
}

int DataSync::update_type(XData* data, int8_t type)
{
    Linker* parent = data;
    while (parent->parent())
    {
        parent = parent->parent();
    }

    std::shared_ptr<IDataSource> source = dynamic_cast<XData*>(parent)->source();
    if (!source) return 0;

    size_t pos = data->get_pos();
    source->Write(pos, 4, &type, 1);
    return 0;
}





