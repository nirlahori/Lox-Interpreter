#include "object.hpp"

Object::Object() = default;

Object::Object(std::nullptr_t) :
    obj_ptr{nullptr}
{}
Object::Object(const Object& other){
    if(!other.obj_ptr){
        obj_ptr = nullptr;
    }
    else{
        obj_ptr = other.obj_ptr->clone();
    }
}
Object& Object::operator= (const Object& other){
    if(!other.obj_ptr){
        obj_ptr = nullptr;
    }
    else{
        obj_ptr = other.obj_ptr->clone();
    }
    return *this;
}
Object::Object(Object&& other) :
    obj_ptr{std::move(other.obj_ptr)}
{
    other.obj_ptr.reset();
}
Object& Object::operator= (Object&& other){
    obj_ptr = std::move(other.obj_ptr);
    other.obj_ptr.reset();
    return *this;
}
bool Object::operator== (std::nullptr_t nullobj){
    return obj_ptr == nullobj;
}
Object::operator std::string () const{
    return obj_ptr->to_str();
}
Object::operator bool() const{
    if(obj_ptr.get() && obj_ptr.get()->to_str() == "0"){
        return false;
    }
    return obj_ptr.get();
}
std::string Object::underlying_type() const{
    return obj_ptr->underlying_type();
}

LoxCallable *Object::as_callable()
{
    return obj_ptr->as_callable();
}
