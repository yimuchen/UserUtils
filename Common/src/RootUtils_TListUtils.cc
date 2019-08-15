#include "TList.h"

#include <stack>

namespace usr {

bool
MoveObjectToBefore( TList&         list,
                    const TObject& target,
                    const TObject& before )
{
  struct objopt
  {
    TObject*    obj;
    std::string opt;
  };

  // Finding the link to where the targets should be moved.
  TObjLink* beforelink = list.FirstLink();

  while( beforelink && beforelink->GetObject() != &before ){
    beforelink = beforelink->Next();
  }

  if( !beforelink ){ return false; }
  beforelink = beforelink->Prev();

  // Flushing all the stuff behind into a target position.
  std::stack<objopt> otherlinks;
  std::stack<objopt> targetlinks;

  TObjLink* link = list.LastLink();

  while( link != beforelink ){
    const objopt temp = { link->GetObject(), link->GetOption() };
    if( link->GetObject() == &target ){
      targetlinks.push( temp );
    } else {
      otherlinks.push( temp );
    }
    link = link->Prev();
    list.RemoveLast();
  }

  // Inserting targets
  while( !targetlinks.empty() ){
    list.Add(
      targetlinks.top().obj,
      targetlinks.top().opt.c_str() );
    targetlinks.pop();
  }

  // Inserting the rest of the stuff.
  while( !otherlinks.empty() ){
    list.Add(
      otherlinks.top().obj,
      otherlinks.top().opt.c_str() );
    otherlinks.pop();
  }

  return true;

}

} /* namespace usr */