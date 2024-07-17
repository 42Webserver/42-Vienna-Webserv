#include "FilePath.hpp"

void FilePath::setStat(void)
{
	m_statValid = (stat(m_string.c_str(), &m_sb) == 0);
}

std::size_t FilePath::calcLength(void) const
{
	std::size_t	l = 0, currPos = 0, prevPos = 0;

	while (currPos < m_string.length() && (currPos = m_string.find_first_of('/', currPos)) != m_string.npos)
	{
		if (currPos - prevPos > 1)
			++l;
		prevPos = currPos++;
	}
	if (prevPos < m_string.length() - 1)
		++l;
	return (l);
}

FilePath::FilePath(void)
{
	setStat();
}

FilePath::FilePath(const FilePath &other) : m_string(other.m_string), m_sb(other.m_sb), m_statValid(other.m_statValid)
{
}

FilePath::FilePath(const std::string &str) : m_string(str)
{
	setStat();
}

FilePath &FilePath::operator=(const FilePath &other)
{
	if (this != &other)
	{
		m_string = other.m_string;
		m_sb = other.m_sb;
		m_statValid = other.m_statValid;
	}
	return (*this);
}

FilePath &FilePath::operator=(const std::string &str)
{
	m_string.replace(0, -1, str);
	setStat();
	return (*this);
}

FilePath::~FilePath()
{
}

void FilePath::append(const std::string &path)
{
	if (m_string.length() && *(m_string.end() - 1) != '/' && path.length() && *(path.end() - 1) != '/')
		m_string.push_back('/');
	m_string.append(path);
	setStat();
}

void FilePath::append(const std::string &path, std::size_t start, std::size_t n)
{
	if (m_string.length() && *(m_string.end() - 1) != '/' && start + n < path.length() && path.at(start + n) != '/')
		m_string.push_back('/');
	m_string.append(path, start, n);
	setStat();
	
}

void FilePath::clear(void)
{
	m_string.clear();
	setStat();
}

const char *FilePath::c_str() const
{
	return (m_string.c_str());
}

const std::string &FilePath::str() const
{
	return (m_string);
}

void FilePath::remove_filename(void)
{
	std::size_t	startPos;
	startPos = (startPos = m_string.find_last_of('/')) == m_string.npos ? m_string.length() : startPos;
	if (startPos + 1 >= m_string.length())
		return;
	m_string.erase(++startPos, -1);
	setStat();
	
}

void FilePath::remove_last(void)
{
	std::size_t	startPos;
	if (m_string.length() < 2)
		return ;
	startPos = (startPos = m_string.find_last_of('/', m_string.length() - 2)) == m_string.npos ? m_string.length() : startPos;
	if (startPos + 1 >= m_string.length())
		return;
	m_string.erase(startPos, -1);
	setStat();
	
}

void FilePath::replace_filename(const std::string &filename)
{
	remove_filename();
	m_string.append(filename);
	setStat();
}

void FilePath::replace_extension(const std::string &extension)
{
	std::string	name = filename();
	std::size_t	dotPos;
	dotPos = (dotPos = name.find_last_of('.')) == m_string.npos ? name.length() : dotPos;
	name.erase(dotPos, -1);
	if (extension.length() && extension.at(0) != '.')
		name.push_back('.');
	name.append(extension);
	replace_filename(name);
	setStat();
}

std::string FilePath::extension(void) const
{
	std::string	name = filename();
	std::size_t	dotPos;
	dotPos = (dotPos = name.find_last_of('.')) == m_string.npos ? m_string.length() : dotPos;
	name.erase(0, dotPos);
	return (name);
}

std::string FilePath::filename(void) const
{
	std::size_t	startPos;
	startPos = (startPos = m_string.find_last_of('/')) == m_string.npos ? m_string.length() : startPos;
	if (startPos + 1 < m_string.length())
		++startPos;
	return (m_string.substr(startPos));
}

mode_t FilePath::getMode(void) const
{
	return (m_sb.st_mode);
}

bool FilePath::exists(void) const
{
	return (m_statValid);
}

bool FilePath::isFile(void) const
{
	return (S_ISREG(m_sb.st_mode));
}

bool FilePath::isDir(void) const
{
	return (S_ISDIR(m_sb.st_mode));
}

bool FilePath::isExec(void) const
{
	return (S_IXGRP & m_sb.st_mode);
}

bool FilePath::isRead(void) const
{
	return (S_IRUSR & m_sb.st_mode);
}

std::ostream &operator<<(std::ostream &os, const FilePath &fp)
{
	os << fp.str();
	return (os);
}
