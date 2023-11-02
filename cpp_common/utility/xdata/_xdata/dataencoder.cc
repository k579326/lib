

#include "dataencoder.h"

#include <assert.h>

#include <stack>

#include "xdata.h"

#define LENGTH_OF_PH        5
#define LENGTH_OF_Byte_1    (LENGTH_OF_PH + 1)
#define LENGTH_OF_Byte_2    (LENGTH_OF_PH + 2)
#define LENGTH_OF_Byte_4    (LENGTH_OF_PH + 4)
#define LENGTH_OF_Byte_8    (LENGTH_OF_PH + 8)

#pragma pack(push, 1)
struct _DATA_H{
    int l; char t;
};
#pragma pack(pop)

namespace 
{
    struct _StackNode
    {
        XData* data;
        DataEncoder::XDataStream stream;
    };
}


int DataEncoder::encode(const XData* data, XDataStream& stream)
{
    char* ptr = stream.first + stream.second;
    auto type = data->type();

    switch (type)
    {
    case Int8:
    case UInt8:
    case Bool:
    {
        char x = data->as_char();
        _DATA_H dh = {};
        dh.l = LENGTH_OF_Byte_1;
        dh.t = (char)type;
        memcpy(ptr, &dh, LENGTH_OF_PH);
        memcpy(ptr + LENGTH_OF_PH, &x, 1);
        stream.second += LENGTH_OF_Byte_1;
        return LENGTH_OF_Byte_1;
    }
    case Int16:
    case UInt16:
    {
        short x = data->as_short();
        _DATA_H dh = {};
        dh.l = LENGTH_OF_Byte_2;
        dh.t = (char)type;
        memcpy(ptr, &dh, LENGTH_OF_PH);
        memcpy(ptr + LENGTH_OF_PH, &x, 2);
        stream.second += LENGTH_OF_Byte_2;
        return LENGTH_OF_Byte_2;
    }
    case Int32:
    case UInt32:
    case Float:
    {
        int x = data->as_short();
        _DATA_H dh = {};
        dh.l = LENGTH_OF_Byte_4;
        dh.t = (char)type;
        memcpy(ptr, &dh, LENGTH_OF_PH);
        memcpy(ptr + LENGTH_OF_PH, &x, 4);
        stream.second += LENGTH_OF_Byte_4;
        return LENGTH_OF_Byte_4;
    }
    case Int64:
    case UInt64:
    case Double:
    {
        int64_t x = data->as_short();
        _DATA_H dh = {};
        dh.l = LENGTH_OF_Byte_8;
        dh.t = (char)type;
        memcpy(ptr, &dh, LENGTH_OF_PH);
        memcpy(ptr + LENGTH_OF_PH, &x, 8);
        stream.second += LENGTH_OF_Byte_8;
        return LENGTH_OF_Byte_8;
    }
    case String:
    {
        std::string s = data->as_string();
        s.append(1, 0);
        size_t len = LENGTH_OF_PH + s.size();
        _DATA_H dh = {};
        dh.l = (int)len;
        dh.t = (char)type;
        
        memcpy(ptr, &dh, LENGTH_OF_PH);
        memcpy(ptr + LENGTH_OF_PH, s.c_str(), s.size());

        stream.second += len;
        return (int)len;
    }
    case Dict:
    case Array:
    {
        int len = LENGTH_OF_PH;
        stream.second += len;
        for (auto child = data->first_child(); child != nullptr; child = child->next_sibling())
        {
            len += encode(child, stream);
        }

        _DATA_H dh = {};
        dh.l = len;
        dh.t = (char)type;
        memcpy(ptr, &dh, LENGTH_OF_PH);
        return len;
    }
    case Pair:
    {
        char* string = ptr + LENGTH_OF_PH;
        int len = LENGTH_OF_PH + (int)data->as_pair().first.size() + 1;
        stream.second += len;
        len += encode(data->as_pair().second, stream);
        
        _DATA_H dh = {};
        dh.l = len;
        dh.t = (char)type;
        memcpy(ptr, &dh, LENGTH_OF_PH);
        strcpy(string, data->as_pair().first.c_str());
        return len;
    }
    case None:
    {
        _DATA_H dh = {};
        dh.l = LENGTH_OF_PH;
        dh.t = (char)type;

        stream.second += LENGTH_OF_PH;
        memcpy(ptr, &dh, LENGTH_OF_PH);
        return LENGTH_OF_PH;
    }
    default:
        break;
    }

    return -1;
}


DataEncoder::XDataStream DataEncoder::_decode(const XDataStream& stream, XData** data)
{
    _DATA_H* h = (_DATA_H*)stream.first;

    *data = new XData();
    void* data_pos = (void*)(stream.first + sizeof(_DATA_H));
    void* children_pos = nullptr;
    size_t cl = 0;

    switch (h->t)
    {
    case ElmType::Int8:
        **data = *(int8_t*)data_pos;
        break;
    case ElmType::Int16:
        **data = *(int16_t*)data_pos;
        break;
    case ElmType::Int32:
        **data = *(int32_t*)data_pos;
        break;
    case ElmType::Int64:
        **data = *(int64_t*)data_pos;
        break;
    case ElmType::UInt8:
        **data = *(uint8_t*)data_pos;
        break;
    case ElmType::UInt16:
        **data = *(uint16_t*)data_pos;
        break;
    case ElmType::UInt32:
        **data = *(uint32_t*)data_pos;
        break;
    case ElmType::UInt64:
        **data = *(uint64_t*)data_pos;
        break;
    case ElmType::Float:
        **data = *(float*)data_pos;
        break;
    case ElmType::Double:
        **data = *(double*)data_pos;
        break;
    case ElmType::Bool:
        **data = *(bool*)data_pos;
        break;
    case ElmType::String:
        **data = (char*)(data_pos);
        break;
    case ElmType::Array:
        delete *data;
        *data = new XData(Array);
        break;
    case ElmType::Dict:
        delete *data;
        *data = new XData(Dict);
        break;
    case ElmType::Pair:
        delete *data;
        *data = new XData((char*)data_pos, new XData());
        break;
    case ElmType::None:
        break;
    default:
        abort();
        break;
    }

    if (h->t == ElmType::Pair)
    {
        size_t lnoc = Size(*data) - LENGTH_OF_PH;
        return { stream.first + lnoc, stream.second - lnoc };
    }
    else if (h->t == ElmType::Array || h->t == ElmType::Dict)
    {
        return { stream.first + LENGTH_OF_PH, stream.second - LENGTH_OF_PH };
    }
    else
    {
        // assert(stream.);
    }

    return { 0, 0 };
}


int DataEncoder::decode(const XDataStream& stream, XData** data)
{
    XData* root = nullptr;

    std::stack<_StackNode> node_stack;
    std::unique_ptr<XData> holder;
    
    auto children_stream = _decode(stream, &root);
    if (!root)
    {
        abort();
        return -1;
    }
    holder.reset(root);

    node_stack.push({root, children_stream});

    while (!node_stack.empty())
    {
        _StackNode node = node_stack.top();
        node_stack.pop();

        char* address = node.stream.first;
        int len = (int)node.stream.second;

        while (len > 0)
        {
            int cl = *(int*)address;
            XData* child = nullptr;

            auto csm = _decode({address, cl}, &child);
            if (!child)
                return -1;

            if (node.data->type() == Array)
            {
                node.data->_array_add_element(child);
            }
            else if (node.data->type() == Dict)
            {
                node.data->_dict_add_element(child);
            }
            else if (node.data->type() == Pair)
            {
                delete node.data->_pair_pointer()->second;
                node.data->_pair_pointer()->second = child;
                child->set_parent(node.data);
            }
            else abort();

            child->set_offset(csm.first - node.stream.first);

            if (child->type() == Dict || child->type() == Array || child->type() == Pair)
                node_stack.push({child, csm});

            address += cl;
            len -= cl;
            assert(len >= 0);
        }
    }

    *data = holder.release();
    return 0;
}

size_t DataEncoder::Size(const XData* data)
{
    size_t _sz = 5;
    switch (data->type())
    {
        case Int8:
        case UInt8:
        case Bool:
            _sz += 1;
            break;
        case Int16:
        case UInt16:
            _sz += 2;
            break;
        case Int32:
        case UInt32:
        case Float:
            _sz += 4;
            break;
        case Int64:
        case UInt64:
        case Double:
            _sz += 8;
            break;
        case String:
        {
            const char* ptr = data->as_string();
            _sz += ptr ? (strlen(ptr) + 1) : 1;
            break;
        }
        case Array:
        case Dict:
        {
            for (auto ptr = data->first_child(); ptr != nullptr; ptr = ptr->next_sibling())
            {
                _sz += Size(ptr);
            }
            break;
        }
        case Pair:
        {
            auto pair = data->as_pair();
            _sz += (pair.first.size() + 1 + Size(pair.second));
            break;
        }
        case None:
        {
            break;
        }
        default:
            break;
    }
    return _sz;
}

