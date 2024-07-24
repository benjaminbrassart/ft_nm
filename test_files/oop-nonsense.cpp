#include <iostream>
#include <string>
#include <stdexcept>

class Animal {
public:
	virtual ~Animal() = 0;

public:
	virtual void make_sound() const = 0;
	virtual std::string const &get_name() const = 0;
};

class Dog : public Animal {
private:
	std::string name;

public:
	Dog() : name("default") {}
	Dog(std::string const &name) : name(name) {}
	Dog(Dog const &x) : name(x.name) {}
	Dog &operator=(Dog const &x) { this->name = x.name; return *this; }
	~Dog() override {}

public:
	void make_sound() const override { std::cout << this->name << ": woof!\n"; }
	std::string const &get_name() const override { return this->name; }
};


class Cat : public Animal {
private:
	std::string name;

public:
	Cat() : name("default") {}
	Cat(std::string const &name) : name(name) {}
	Cat(Cat const &x) : name(x.name) {}
	Cat &operator=(Cat const &x) { this->name = x.name; return *this; }
	~Cat() override {}

public:
	void make_sound() const override { std::cout << this->name << ": meow!\n"; }
	std::string const &get_name() const override { return this->name; }
};

void hello() {
	auto cat = Cat("sam");
	auto dog = Cat("peter");

	cat.make_sound();
	dog.make_sound();

	throw std::runtime_error("i hate exceptions");
}

template<class T>
T return_T(T const &t) {
	return t;
}

bool test_hello() {
	if (return_T(42) != 42) {
		return false;
	}

	if (return_T(true) != true) {
		return false;
	}

	if (return_T(std::string("some c++ string")) != "some c++ string") {
		return false;
	}

	try {
		hello();
		return true;
	} catch (std::exception const &e) {
		std::cerr << "hello() threw an exception: " << e.what() << '\n';
		return false;
	}
}
