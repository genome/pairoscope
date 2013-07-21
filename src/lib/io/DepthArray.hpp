#pragma once

#include <bam.h>

#include <algorithm>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>

template<typename DepthType>
class DepthArray {
public:
    typedef DepthType value_type;
    typedef std::vector<value_type> container_type;
    typedef typename container_type::size_type size_type;

    DepthArray()
        : _begin(0)
        , _end(0)
        , _max(0)
    {
    }

    DepthArray(std::string const& sequence_name, size_t begin, size_t end)
        : _max(0)
    {
        setRegion(sequence_name, begin, end);
    }

    // Basics
    value_type const& max_value() const { return _max; }
    size_type size() const { return _depth.size(); }
    bool empty() const { return _depth.empty(); }
    std::string const& sequence_name() { return _sequence_name; }
    size_t begin_pos() const { return _begin; }
    size_t end_pos() const { return _end; }

    void setRegion(std::string const& sequence_name, size_t begin, size_t end) {
        if (end <= begin) {
            throw std::range_error("DepthArray: end must be > begin!");
        }

        _sequence_name = sequence_name;
        _begin = begin;
        _end = end;

        _depth.resize(_end - _begin, 0);
    }

    value_type const& operator[](size_t pos) const {
        validate_position(pos);
        return _depth[pos - _begin];
    }

    value_type const& at_index(size_t idx) const {
        if (idx > _depth.size())
            throw std::range_error("Depth index out of range");
        return _depth[idx];
    }

    void increment(size_t pos, value_type const& value) {
        validate_position(pos);
        value_type& target = _depth[pos - _begin];
        if (std::numeric_limits<value_type>::max() - target < value)
            target = std::numeric_limits<value_type>::max();
        else
            target += value;

        _max = std::max(_max, target);
    }

    int pileupCallback(
        uint32_t tid,
        uint32_t pos,
        int n,
        const bam_pileup1_t *pl
        )
    {
        int mapq_n = 0;
        if (pos >= _begin && pos < _end) {
            //loop over the bases
            int i;
            for(i = 0; i < n; ++i) {
                const bam_pileup1_t *base = pl + i;
                if(!base->is_del) {
                    mapq_n++;
                }
            }
            //set depth at position
            increment(pos, mapq_n);
        }

        return 0;
    }

private:
    void validate_position(size_t pos) const {
        if (pos < _begin || pos >= _end) {
            std::stringstream ss;
            ss << "Request for position '" << pos << " in depth array for "
                "region " << _sequence_name << ":" << _begin << ":" << _end;

            throw std::range_error(ss.str());
        }
    }

private:
    std::string _sequence_name;
    size_t _begin;
    size_t _end;
    container_type _depth;
    value_type _max;
};
