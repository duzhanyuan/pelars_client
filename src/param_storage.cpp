#include "param_storage.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <string>
#include <fstream>


YAML::Node ParameterServer::solveparent(std::string path)
{
	std::stringstream ss(path);
	std::string item;
	YAML::Node c = top;
	YAML::Node parent = top;
	while (std::getline(ss, item, '/')) 
	{
		if(c.IsMap())
		{
			parent = c;
			c = c[item];
		}
		else
		{
			return YAML::Node(); // fail
		}
  	}
  	return parent;
}

void ParameterServer::dump(std::string out_file){

	std::ofstream fout(out_file);
	fout << top;
}

YAML::Node ParameterServer::solve(std::string path)
{
	std::stringstream ss(path[0] == '/' ? path.substr(1) : path);
	std::string item;
	YAML::Node c = top;
	while (std::getline(ss, item, '/')) 
	{
		if(c.IsMap())
		{
			c = c[item];
		}
		else
		{
			return YAML::Node(); // fail
		}
  	}
  	return c;
}

bool ParameterServer::param(std::string path, YAML::Node & v)
{
	YAML::Node s = solve(path);
	if(s)
	{
		v = s;
		return true;
	}
	else
	{
		return false;	
	}
}

bool ParameterServer::append(std::string path, std::string prefix)
{
	YAML::Node q = YAML::LoadFile(path);
	if(!q.IsMap())
	{
		std::cerr << "loaded file is not a map\n";
		return false;
	}
	YAML::Node s = prefix.empty() ? top : solve(prefix);
	if(!s)
	{
		// create the prefix ... TBD
	}
	// q has to be a dictionary
	if(!top)
	{
		top = q;
	}
	else
	{
		for(auto it: q)
		{			
			top[it.first] = it.second;
		}
	}
	return true;
}

/*
int main(int argc, char const *argv[])
{
	ParameterServer srv;
	srv.append("config.yaml"); // TODO add prefix
	srv.append("config2.yaml");

	int x = 0;
	ParameterSpace ps(srv,"ciao");
	ps.param("wow",x,10); // ciao/wow

	ParameterSpace ps(srv,"ciao2");
	std::string q;
	ps.param("ciao2/wow2",q,std::string("ciao"));

	return 0;
}
*/