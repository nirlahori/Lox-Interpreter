#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <memory>
#include <string>
#include <ostream>

class  LoxFunction;
struct LoxCallable;


class Object{

private:

    class ObjectBase{

        public:
        using type = void;
        virtual std::unique_ptr<ObjectBase> clone() = 0;
        virtual std::string to_str() = 0;
        virtual std::string underlying_type() = 0;
        virtual LoxCallable* as_callable() = 0;
        virtual ~ObjectBase() = default;
    };


    template<typename ObjectType>
    class ObjectImpl : public ObjectBase{

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
                if constexpr(std::is_same_v<LoxFunction, ObjectType>){
                    return data.to_string();
                }
                else{
                    return std::to_string(data);
                }
            }
        }

        std::string underlying_type(){
            if(std::is_same<ObjectType, bool>::value){
                return "bool";
            }
            else if(std::is_same<ObjectType, std::string>::value){
                return "string";
            }
            else if(std::is_same<ObjectType, double>::value){
                return "double";
            }
            else if(std::is_same<ObjectType, LoxFunction>::value){
                return "lox_function";
            }
        }

        LoxCallable* as_callable(){
            if constexpr(std::is_base_of<LoxCallable, ObjectType>::value){
                return &data;
            }
            else{
                return nullptr;
            }
        }

        ~ObjectImpl() = default;

    };


    std::unique_ptr<ObjectBase> obj_ptr;


public:

    Object();

    template<typename T>
    Object(const T& _val) :
        obj_ptr{std::make_unique<ObjectImpl<T>>(_val)}
    {}
    Object(std::nullptr_t);
    Object(const Object& other);
    Object& operator= (const Object& other);

    Object(Object&& other);

    Object& operator= (Object&& other);
    bool operator== (std::nullptr_t nullobj);

    operator std::string () const;
    operator bool() const;


    friend std::ostream& operator<< (std::ostream& os, Object obj){
        if(obj.obj_ptr){
            os << obj.obj_ptr->to_str();
        }
        else{
            os << nullptr;
        }
        return os;
    }

    std::string underlying_type() const;
    LoxCallable* as_callable();

    friend bool operator== (const Object& lhs, const Object& rhs){
        if(lhs.underlying_type() == "double" && rhs.underlying_type() == "double"){
            return std::stod(lhs) == std::stod(rhs);
        }
        else if(lhs.underlying_type() == "string" && rhs.underlying_type() == "string"){
            return static_cast<std::string>(lhs) == static_cast<std::string>(rhs);
        }
        else if(lhs.underlying_type() == "bool" && rhs.underlying_type() == "bool"){
            return static_cast<bool>(lhs) == static_cast<bool>(rhs);
        }
        else{
            return false;
        }
    }

    friend bool operator== (const Object& lhs, const double& rhs){
        if(lhs.underlying_type() == "double"){
            return std::stod(lhs) == rhs;
        }
        return false;
    }
};


#endif // OBJECT_HPP
