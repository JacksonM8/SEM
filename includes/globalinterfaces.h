#ifndef GLOBALINTERFACES_H
#define GLOBALINTERFACES_H

//Interface for a standard In Event Port
template <class T> class InEventPort{
    public:
    //protected:
        virtual void rx_(T*) = 0;
};

//Interface for a standard Out Event Port
template <class T> class OutEventPort{
    public:
    //protected:
        virtual void tx_(T*) = 0;
};

#endif //GLOBALINTERFACES_H
