#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include "arguments.hpp"
#include "graph.hpp"
#include "graph_serialization.hpp"
#include "matrixes.hpp"

class serializer
{
  const arguments &args;
  const Graph &g;
  const fp_matrix &tm;

public:
  serializer(const arguments &args, const Graph &g, const fp_matrix &tm);

  void operator()(const pt_matrix &ptm) const;
  void operator()(const pt_matrix &ptm, int iteration) const;
};

#endif /* SERIALIZER_HPP */
