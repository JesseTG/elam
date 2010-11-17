//d-ptr header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef DPTR_H
#define DPTR_H

/** \page dptr Automatic d-Pointers
to be written

\see dptr.h
*/

/** \file "dptr.h"
Automatic d-Pointers Header File

Please see \ref dptr "Automatic d-Pointers"
for a tutorial.
*/

/** \brief Expands to the fully qualified name of the d-pointer class.
\param Class the fully qualified name of the class the d-pointer was declared in.*/
#define DPTR_CLASS_NAME(Class) Class::Private
///Expands to the local name of d-pointer classes (Private).
#define DPTR_NAME Private
///Expands to the local name of the d-pointer wrapper class (DPrivate).
#define DPTR_WRAPPER_NAME DPrivate

/** \brief Declares a smart shared or non-shared d-pointer, to be used inside class declaration.

It also declares the internal nested classes DPrivate and Private - Private is the actual d-pointer class, while DPrivate is a wrapper that automatically allocates, copies and deallocates the d-pointer.

The wrapper DPointer class contains these methods:
   - constructor, copy constructor for creating instances of Private
    - used by automatic and explicit constructors of the containing class
   - destructor for deallocation of Private
    - used by the destructor of the containing class
   - assignment operator to copy the content of Private
    - used by the assignment operator of the containing class
   - pointer operator to actually access the Private data

You can use DEFINE_DPTR to define the necessary methods for a non-shared d-pointer or DEFINE_SHARED_DPTR if you want to share d-pointer data between instances of the containing class. It is recommended that non-shared d-pointer classes (Private) are derived from DPtr and the shared variants be derived from SharedDPtr.

The d-pointer class Private is only forward declared, you have to fully declare and implement it in the code where you are using it, i.e. where you are implementing the containing class.

\param dp name of the d-pointer*/
#define DECLARE_DPTR(dp) \
 private:\
 class Private; \
 class DPrivate{\
  public:DPrivate();DPrivate(const DPrivate&);~DPrivate();\
  DPrivate&operator=(const DPrivate&);\
  const Private*operator->()const{return d;}\
  Private*operator->(){return d;}\
  DPrivate clone()const;\
  private:Private*d;\
 }; \
 DPrivate dp;

 /** \brief Alias for DECLARE_DPTR for convenience.

\param dp name of the d-pointer*/
#define DECLARE_SHARED_DPTR(dp) DECLARE_DPTR(dp)

/** \brief Base class of non-shared d-pointers.

Use in conjunction with DECLARE_DPTR and DEFINE_DPTR */
class DPtr
{
	public:
		///instantiates a non-shared d-pointer
		DPtr(){}
		///deletes a non-shared d-pointer
		virtual ~DPtr(){}
};

/** \brief Creates definitions for methods of the non-shared d-pointer wrapper. 

This variant is not shared between instances of the containing class.

To be used in implementation where the actual d-pointer class is implemented.

\param Class the base class within which the d-pointer was declared*/
#define DEFINE_DPTR(Class) \
 Class::DPrivate::DPrivate(){d=new Class::Private;}\
 Class::DPrivate::DPrivate(const Class::DPrivate&dp){d=new Class::Private(*(dp.d));}\
 Class::DPrivate::~DPrivate(){delete d;}\
 Class::DPrivate Class::DPrivate::clone()const{DPrivate r;*(r.d)=*d;return r;}\
 Class::DPrivate& Class::DPrivate::operator=(const Class::DPrivate&dp)\
 {*d=*(dp.d);return *this;}

/** \brief Base class of shared d-pointers.

Use in conjunction with DECLARE_SHARED_DPTR and DEFINE_SHARED_DPTR */
class SharedDPtr
{
	private:
		int cnt;
	public:
		///instantiates a shared d-pointer
		SharedDPtr(){cnt=1;}
		///deletes a shared d-pointer
		virtual ~SharedDPtr(){}
		///called by the wrapper to attach to a new instance
		virtual void attach(){cnt++;}
		///called by the wrapper to detach from an instance
		virtual void detach(){cnt--;if(cnt==0)delete this;}
};

/** \brief Defines the methods of the shared d-pointer wrapper.

This implements the shared version of the d-pointer wrapper.
To be used in implementation where the actual d-pointer class is implemented.

\param Class the base class within which the d-pointer was declared*/
#define DEFINE_SHARED_DPTR(Class) \
 Class::DPrivate::DPrivate(){d=new Class::Private;}\
 Class::DPrivate::DPrivate(const DPrivate&dp){d=dp.d;d->attach();}\
 Class::DPrivate::~DPrivate(){d->detach();}\
 Class::DPrivate Class::DPrivate::clone()const{DPrivate r;*(r.d)=*d;return r;}\
 Class::DPrivate& Class::DPrivate::operator=(const DPrivate&dp)\
 {d->detach();d=dp.d;d->attach();return *this;}


#endif
