#pragma once

template<class T>
class Singleton
{
public:
    static T& GetInstance()
    {
      static T instance;
      return instance;
    }

    Singleton(Singleton const&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton const&) = delete;
    Singleton& operator=(Singleton&&) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};