/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Base.h>

template <typename T>
struct SArray /* : public std::vector<T> */ {
    /* allocator<_Ty>*/ void* allocator;   // MSVC 6.0 STL vector. Como el original: el constructor no lo escribe (0x5EDF10)
    /* iterator      */ T*    _First{};    // ptr to array (=> `begin()`)
    /* iterator      */ T*    _Last{};     // ptr to past-the-end of all active elements (=> `end()`)
    /* iterator      */ T*    _End{};      // ptr to past-the-end of array (=> `begin() + capacity()`)
public:
    using value_type = T;

    SArray() noexcept = default;
    SArray(const SArray& o) :
        SArray{}
    {
        if (o.empty() || &o == this) {
            return;
        }
        _First = Allocate(o.capacity());
        std::uninitialized_copy(o.begin(), o.end(), _First);
        _Last = _First + o.size();
        _End  = _First + o.capacity();
    }
    SArray(SArray&& other) noexcept :
        SArray()
    {
        swap(*this, other);
    }

    ~SArray() {
        clear();
    }

    SArray& operator=(SArray o) {
        swap(*this, o);
        return *this;
    }

    friend void swap(SArray<T>& a, SArray<T>& b) {
        using std::swap;

        swap(a._First, b._First);
        swap(a._Last, b._Last);
        swap(a._End, b._End);
    }

    auto size() const { return _First ? (size_t)(_Last - _First) : 0u; }
    auto capacity() const { return _First ? (size_t)(_End - _First) : 0u; }
    auto begin() const { return _First; }
    auto end() const { return _Last; }
    auto empty() const { return size() == 0; }
    auto front() const { assert(!empty()); return *_First; }
    auto back() const { assert(!empty()); return *(_Last - 1); }
    void clear() { // in this implementation for the sake of simplicity `clear` actually deallocates all memory and destructs all items
        rng::destroy(begin(), end());
        Deallocate(std::exchange(_First, nullptr));
        _Last = nullptr;
        _End  = nullptr;
    }
    void reserve(size_t newCap) {
        const auto oldCap = capacity();
        if (newCap <= oldCap) {
            return;
        }
        const auto currSize = size();

        // Alloc new storage
        T* const dst = Allocate(newCap);
        assert(dst);
        
        // Move items over (If any)
        std::uninitialized_move(begin(), end(), dst);
        rng::destroy(begin(), end());

        // De-allocate old storage
        Deallocate(std::exchange(_First, dst));

        // Re-calculate pointers
        _First = dst;
        _Last  = dst + currSize;
        _End   = dst + newCap;
    }
    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (!_First || _Last == _End) {
            // Como el original (vector::_Insert_n, p. ej. 0x5EDA50): capacidad + capacidad / 2 y, si no
            // basta, tamaño + 1 (con 0 o 1 elementos crece a tamaño + 1).
            const auto cap = capacity();
            reserve(std::max<size_t>(size() + 1, cap + cap / 2));
        }
        std::construct_at(_Last++, std::forward<Args>(args)...);
    }
    void erase(T* first, T* last) {
        assert(first >= _First && first < _Last);
        assert(last >= first && last <= _Last);
        // Como el original: desplaza hacia delante el resto de elementos (antes era move_backward,
        // que escribía delante de `first`)
        std::move(last, _Last, first);
        rng::destroy(_Last - (last - first), _Last);
        _Last -= (last - first);
    }
    void erase(T* iter) {
        erase(iter, iter + 1);
    }

    auto operator[](size_t idx) const { assert(idx < size()); return _First[idx]; }

private:
    // Como el original: el allocator de MSVC usa operator new / operator delete (0x82119A / 0x8214BD),
    // no new[] / delete[], y no llama a operator delete con un puntero nulo.
    static T* Allocate(size_t n) { return static_cast<T*>(::operator new(n * sizeof(T))); }
    static void Deallocate(T* p) { if (p) { ::operator delete(p); } }
};
VALIDATE_SIZE(SArray<int32>, 0x10);
