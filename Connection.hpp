#ifndef CONNECTION_H
# define CONNECTION_H

class Connection
{
public:
	Connection(void);
	Connection(const Connection& a_other);
	Connection& operator=(const Connection& a_other);
	~Connection();
};

#endif // !CONNECTION_H
