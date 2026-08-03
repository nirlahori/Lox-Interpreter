#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <memory>
#include <string>

class Object{

private:

    class ObjectBase{
        public:
        virtual std::unique_ptr<ObjectBase> clone() = 0;
        virtual std::string to_str() = 0;
        virtual ~ObjectBase() = default;
    };

    template<typename ObjectType>
    class ObjectImpl : public ObjectBase{
        private:
        ObjectType data;

        public:
        ObjectImpl() = default;
        ObjectImpl(const ObjectType& _data) :
            data{_data}
            {}

        std::unique_ptr<ObjectBase> clone() {
            return std::make_unique<ObjectImpl<ObjectType>>(*this);
        }

        std::string to_str(){
            if constexpr(std::is_same_v<std::string, ObjectType>){
                return data;
            }
            else{
                return std::to_string(data);
            }
        }

        ~ObjectImpl() = default;

    };

    std::unique_ptr<ObjectBase> obj_ptr;


public:

    Object() = default;

    template<typename T>
    Object(const T& _val) :
        obj_ptr{std::make_unique<ObjectImpl<T>>(_val)}
        {}
    Object(std::nullptr_t _ptr) :
        obj_ptr{nullptr}
        {}

    Object(const Object& other){
        if(!other.obj_ptr){
            obj_ptr = nullptr;
        }
        else{
            obj_ptr = other.obj_ptr->clone();
        }
    }

    Object& operator= (const Object& other){
        Object tmp (other);
        std::swap(tmp, *this);
        return *this;
    }

    Object(Object&& other) :
        obj_ptr{std::move(other.obj_ptr)}
    {
        other.obj_ptr.reset();
    }

    bool operator== (std::nullptr_t nullobj){
        return obj_ptr == nullobj;
    }

    operator std::string () const{
        return obj_ptr->to_str();
    }

    operator bool() const{
        return obj_ptr.get();
    }

};

#endif // OBJECT_HPP
