/**
 *  \file detail/engine_visitor.hpp
 *
 *  Copyright 2014 Michael Caisse : ciere.com
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef BOOST_BOOSTACHE_VM_DETAIL_ENGINE_VISITOR_HPP
#define BOOST_BOOSTACHE_VM_DETAIL_ENGINE_VISITOR_HPP

#include <boost/boostache/vm/engine_ast.hpp>
#include <boost/boostache/vm/detail/foreach.hpp>


////////// some test hackery ////////////////////////

namespace boost { namespace boostache { namespace extension
{
   template <typename Stream, typename Object>
   void render(Stream & stream, Object const & v)
   {
      stream << v;
   }

   template <typename T>
   bool test(std::string const & name, T const & context);

   template< typename Stream, typename T >
   void render(Stream & stream, T const & context, std::string const & name);
}}}

/////////////////////////////////////////////////////


namespace boost { namespace boostache { namespace vm { namespace detail
{
   template <typename Stream, typename Context>
   class engine_visitor_base
   {
   public:
      typedef void result_type;

      engine_visitor_base(Stream & s, Context const & c)
         : stream(s)
         , context(c)
      {}

      void operator()(ast::undefined) const
      {}

      void operator()(ast::literal const & v) const
      {
         std::cout << "LITERAL" << std::endl;
         using boost::boostache::extension::render;
         render(stream,v.value);
      }

      void operator()(ast::variable const & v) const
      {}

      void operator()(ast::render const & v) const
      {
         std::cout << "RENDER(" << v.name << ")" << std::endl;
         using boost::boostache::extension::render;
         render(stream,context,v.name);
      }

      void operator()(ast::for_each const & v) const
      {
         std::cout << "--> FOREACH(" << v.name << ")" << std::endl;
         using boost::boostache::vm::detail::foreach;
         foreach(stream,v,context);
         std::cout << "<-- FOREACH(" << v.name << ")" << std::endl;
      }

      void operator()(ast::if_then_else const & v) const
      {
         std::cout << "--> IFTHENELSE(" << v.condition_.name << ")" << std::endl;
         using boost::boostache::extension::test;
         if(test(v.condition_.name,context))
         {
            std::cout << "THEN" << std::endl;
            boost::apply_visitor(*this, v.then_);
         }
         else
         {
            std::cout << "ELSE" << std::endl;
            boost::apply_visitor(*this, v.else_);
         }
         std::cout << "<-- IFTHENELSE(" << v.condition_.name << ")" << std::endl;
      }

      void operator()(ast::node_list const & nodes) const
      {
         std::cout << "NODELIST" << std::endl;
         for(auto const & node : nodes.nodes)
         {
            boost::apply_visitor(*this, node);
         }
      }

      void operator()(ast::node const & v) const
      {
         std::cout << "NODE" << std::endl;
         boost::apply_visitor(*this, v);
      }

   private:
      Stream & stream;
      Context const & context;
   };   

}}}}


#endif
