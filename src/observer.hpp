
/**
The Observer Design Pattern

To use this pattern, any subject class of yours should:

- include the SUBJECT preprocessor macro in its beginning, like
@code
class <YOUR_SUBJECT_CLASS_HERE> {
SUBJECT
	<YOUR_STUFF_HERE>
};
@endcode

- and call
@code
subject.init(<YOUR_SUBJECT_CLASS_EVENT_AMOUNT>);
@endcode
in its constructors, or somewhere.

@file observer.hpp
@author Matheus Costa de Sousa Carvalho Pimenta
*/

#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <list>

/// The macro for a class to be a subject.
#define SUBJECT																									\
protected:																										\
	observer::Subject subject;																					\
public:																											\
	void connect(int event_type, observer::handler_type handlerfp) {											\
		subject.connect(event_type, handlerfp);																	\
	}																											\
																												\
	template <class obs_type>																					\
	void connect(int event_type, obs_type* observer, typename observer::handler<obs_type>::type handlerfp) {	\
		subject.connect(event_type, observer, handlerfp);														\
	}																											\
																												\
	void disconnect(observer::handler_type handlerfp) {															\
		subject.disconnect(handlerfp);																			\
	}																											\
																												\
	void disconnect(int event_type, observer::handler_type handlerfp) {											\
		subject.disconnect(event_type, handlerfp);																\
	}																											\
																												\
	template <class obs_type>																					\
	void disconnect(obs_type* observer) {																		\
		subject.disconnect(observer);																			\
	}																											\
																												\
	template <class obs_type>																					\
	void disconnect(int event_type, obs_type* observer) {														\
		subject.disconnect(event_type, observer);																\
	}

/// All names of the pattern are here.
namespace observer {

/// Class to identify and hold event data.
class Event {
private:
	int type_;
public:
	Event(int type);	///< Constructor to assign the type.
	virtual ~Event();	///< Polymorphic destructor.
	
	int type() const;	///< Access to the type of the event.
};

/// Callback type for functions or static member functions.
typedef void (*handler_type)(const Event& event, bool& stop);

/// A disguise for a typedef "template" for the callbacks.
template <class obs_type>
struct handler {
	typedef void (obs_type::*type)(const Event& event, bool& stop);
};

/// Class for the subject "mechanism", to be held by classes that are using the above macro.
class Subject {
private:
	struct Observer {
		int stack_id;
		
		Observer();
		virtual ~Observer();
		
		virtual void callHandler(const Event& event, bool& stop) = 0;
		
		virtual Observer* clone() = 0;
	};
	
	struct ObserverFunction : public Observer {
		handler_type handlerfp;
		
		ObserverFunction(handler_type handlerfp);
		~ObserverFunction();
		
		void callHandler(const Event& event, bool& stop);
		
		Observer* clone();
	};
	
	template <class obs_type>
	struct ObserverObject : public Observer {
		obs_type* observer;
		typename handler<obs_type>::type handlerfp;
		
		ObserverObject(obs_type* observer, typename handler<obs_type>::type handlerfp) : observer(observer), handlerfp(handlerfp) {}
		~ObserverObject() {}
		
		void callHandler(const Event& event, bool& stop) { ((*observer).*(handlerfp))(event, stop); }
		
		Observer* clone() { return new ObserverObject<obs_type>(*this); }
	};
	
	int n_events;
	std::list<Observer*>* observers;
	bool* broadcasting;
public:
	/// Empty constructor. You'll probably never call it explicitly.
	Subject();
	
	/// Copy constructor. You'll probably never call it explicitly.
	Subject(const Subject& rv);
	
	/// Empty destructor. You'll probably never call it explicitly.
	~Subject();
	
	/// Copy assignment operator. You'll probably never call it explicitly.
	Subject& operator=(const Subject& rv);
private:
	void copy(const Subject& rv);
	void clear();
public:
	/// Initializes the subject with the number of event types it is going to have. Needs to be called once and before doing anything else.
	void init(int n_events);
	
	/// Broadcasts an event.
	void broadcast(const Event& event);
	
	/// Creates an observer for the specified event type, with the observer function callback.
	void connect(int event_type, handler_type handlerfp);
	
	/// Creates an observer for the specified event type, with the observer object and the handlerfp callback.
	template <class obs_type>
	void connect(int event_type, obs_type* observer, typename handler<obs_type>::type handlerfp);
	
	/// Destroys all the observers for the observer function.
	void disconnect(handler_type handlerfp);
	
	/// Destroys the observer for the specified event type.
	void disconnect(int event_type, handler_type handlerfp);
	
	/// Destroys all the observers for the observer object.
	template <class obs_type>
	void disconnect(obs_type* observer);
	
	/// Destroys the observer for the specified event type.
	template <class obs_type>
	void disconnect(int event_type, obs_type* observer);
};

/// This class implements a stack for all the connections made with this pattern, to suspend the callbacks with a rule based in the timeline.
class Stack {
private:
	enum {
		PUSH,
		POP
	};
	
	static bool is_init;
	static int id_;
	static Subject subject;
	
	static bool init();
public:
	/// Returns the current stack id.
	static int id();
	
	/// Broadcasts the push event and increments the stack id.
	static void push();
	
	/// Broadcasts the pop event and decrements the stack id.
	static void pop();
	
	/// Method to connect an object to the push event.
	template <class obs_type>
	static void connect(obs_type* observer, typename handler<obs_type>::type push_handler, typename handler<obs_type>::type pop_handler);
	
	/// Method to disconnect an object from the push event.
	template <class obs_type>
	static void disconnect(obs_type* observer);
};

}	// namespace observer end

// =============================================================================
// Observer Subject Class
// =============================================================================

template <class obs_type>
void observer::Subject::connect(int event_type, obs_type* observer, typename handler<obs_type>::type handlerfp) {
	if (!n_events)
		return;
	
	// searches the observer to replace the callback and set the flag value to true
	bool stop = false;
	for (std::list< Observer* >::iterator it = observers[event_type].begin(); (it != observers[event_type].end()) && (!stop); ++it) {
		if (((ObserverObject<obs_type>*)(*it))->observer == observer) {
			stop = true;
			((ObserverObject<obs_type>*)(*it))->handlerfp = handlerfp;
		}
	}
	// only pushes a new observer if the value of the flag is still false
	if (!stop) {
		observers[event_type].push_back(new ObserverObject<obs_type>(observer, handlerfp));
	}
}

template <class obs_type>
void observer::Subject::disconnect(obs_type* observer) {
	if (!n_events)
		return;
	
	for (int i = 0; i < n_events; ++i) {
		// searches the observer in the current event type
		for (std::list<Observer*>::iterator it = observers[i].begin(); it != observers[i].end(); ++it) {
			// erases if it is found and breaks the loop for this method
			if (((ObserverObject<obs_type>*)(*it))->observer == observer) {
				delete *it;
				observers[i].erase(it);
				break;
			}
		}
	}
}

template <class obs_type>
void observer::Subject::disconnect(int event_type, obs_type* observer) {
	if (!n_events)
		return;
	
	// searches the observer in the specified event type
	for (std::list<Observer*>::iterator it = observers[event_type].begin(); it != observers[event_type].end(); ++it) {
		// erases if it is found and returns this method
		if (((ObserverObject<obs_type>*)(*it))->observer == observer) {
			delete *it;
			observers[event_type].erase(it);
			return;
		}
	}
}

// =============================================================================
// Observer Stack Class
// =============================================================================

template <class obs_type> 
void observer::Stack::connect(obs_type* observer, typename handler<obs_type>::type push_handler, typename handler<obs_type>::type pop_handler) {
	init();
	subject.connect(PUSH, observer, push_handler);
	subject.connect(POP, observer, pop_handler);
}

template <class obs_type> 
void observer::Stack::disconnect(obs_type* observer) {
	subject.disconnect(observer);
}

#endif
