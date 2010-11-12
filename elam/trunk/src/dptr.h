//d-ptr header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef DPTR_H
#define DPTR_H

/**Declares a smart d-pointer, to be used inside class declaration.

It also declares the internal nested classes DPrivate and Private - Private is the actual d-pointer class, while DPrivate is a wrapper that automatically allocates and deallocates the d-pointer.

\param dp name of the d-pointer*/
#define DECLARE_DPTR(dp) \
 class Private; \
 class DPrivate{\
  public:DPrivate();DPrivate(const DPrivate&);~DPrivate();\
  DPrivate&operator=(const DPrivate&);\
  Private*operator->()const{return d;}private:Private*d;\
 }; \
 DPrivate dp;

/**Creates definitions for methods of the d-pointer wrapper, to be used in implementation where the actual d-pointer class is implemented.

\param Class the base class within which the d-pointer was declared*/
#define DEFINE_DPTR(Class) \
 Class::DPrivate::DPrivate(){d=new Class::Private;};\
 Class::DPrivate::DPrivate(const Class::DPrivate&dp){d=new Class::Private(*(dp.d));};\
 Class::DPrivate::~DPrivate(){delete d;}\
 Class::DPrivate& Class::DPrivate::operator=(const Class::DPrivate&dp){*d=*(dp.d);return *this;}

/**Expands to the fully qualified name of the d-pointer class.
\param Class the fully qualified name of the class the d-pointer was declared in.*/
#define DPTR_CLASS_NAME(Class) Class::Private
///Expands to the local name of d-pointer classes (Private).
#define DPTR_NAME Private

/**Declares a smart shared d-pointer, to be used inside class declaration.

It also declares the internal nested classes DPrivate and Private - Private is the actual d-pointer class, while DPrivate is a wrapper that automatically allocates and deallocates the d-pointer.

The d-pointer class must be derived from SharedDPtr in order to work with this macro.

\param Class the base class within which the d-pointer was declared
\param dp name of the d-pointer*/
//public:\
// Class& operator=(const Class&c){dp=c.dp;return *this;} 
#define DECLARE_SHARED_DPTR(dp) \
private:\
 class Private; \
 class DPrivate{public:\
   DPrivate();DPrivate(const DPrivate&);~DPrivate();\
   Private*operator->()const{return d;}\
   DPrivate& operator=(const DPrivate&);\
  private:\
   Private*d;\
 }; \
 DPrivate dp;

/**Base class of shared d-pointers. Use in conjunction with DECLARE_SHARED_DPTR and DEFINE_SHARED_DPTR */
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

/**Defines the methods of the shared d-pointer wrapper, to be used in implementation where the actual d-pointer class is implemented.

\param Class the base class within which the d-pointer was declared*/
#define DEFINE_SHARED_DPTR(Class) \
 Class::DPrivate::DPrivate(){d=new Class::Private;}\
 Class::DPrivate::DPrivate(const DPrivate&dp){d=dp.d;d->attach();}\
 Class::DPrivate::~DPrivate(){d->detach();}\
 Class::DPrivate& Class::DPrivate::operator=(const DPrivate&dp){d->detach();d=dp.d;d->attach();return *this;}


#endif
