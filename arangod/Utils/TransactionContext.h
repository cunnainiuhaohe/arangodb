////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_UTILS_TRANSACTION_CONTEXT_H
#define ARANGOD_UTILS_TRANSACTION_CONTEXT_H 1

#include "Basics/Common.h"
#include "VocBase/voc-types.h"

#include <velocypack/Options.h>

struct TRI_document_collection_t;
struct TRI_transaction_t;
struct TRI_vocbase_t;

namespace arangodb {
namespace velocypack {
struct CustomTypeHandler;
}

class CollectionNameResolver;
class DocumentDitch;

class TransactionContext {
 public:
  TransactionContext(TransactionContext const&) = delete;
  TransactionContext& operator=(TransactionContext const&) = delete;

 protected:

  //////////////////////////////////////////////////////////////////////////////
  /// @brief create the context
  //////////////////////////////////////////////////////////////////////////////

  explicit TransactionContext(TRI_vocbase_t* vocbase);

 public:

  //////////////////////////////////////////////////////////////////////////////
  /// @brief destroy the context
  //////////////////////////////////////////////////////////////////////////////

  virtual ~TransactionContext();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief factory to create a custom type handler, not managed
  //////////////////////////////////////////////////////////////////////////////

  static arangodb::velocypack::CustomTypeHandler* createCustomTypeHandler(
           TRI_vocbase_t*,
           arangodb::CollectionNameResolver const*);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the vocbase
  //////////////////////////////////////////////////////////////////////////////

  TRI_vocbase_t* vocbase() const { return _vocbase; }
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the context contains a resolver
  //////////////////////////////////////////////////////////////////////////////

  inline bool hasResolver() const { return _resolver != nullptr; }
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief orders a custom type handler
  //////////////////////////////////////////////////////////////////////////////

  std::shared_ptr<velocypack::CustomTypeHandler> orderCustomTypeHandler();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief order a document ditch for the collection
  /// this will create one if none exists
  //////////////////////////////////////////////////////////////////////////////

  DocumentDitch* orderDitch(TRI_document_collection_t*);
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the ditch for a collection
  /// this will return a nullptr if no ditch exists
  //////////////////////////////////////////////////////////////////////////////
  
  DocumentDitch* ditch(TRI_voc_cid_t) const;
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief get velocypack options with a custom type handler
  //////////////////////////////////////////////////////////////////////////////
  
  arangodb::velocypack::Options* getVPackOptions();
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief unregister the transaction
  /// this will save the transaction's id and status locally
  //////////////////////////////////////////////////////////////////////////////

  void storeTransactionResult(TRI_voc_tid_t, bool);
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the resolver
  //////////////////////////////////////////////////////////////////////////////

  virtual CollectionNameResolver const* getResolver() = 0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief get parent transaction (if any)
  //////////////////////////////////////////////////////////////////////////////

  virtual struct TRI_transaction_t* getParentTransaction() const = 0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the transaction is embeddable
  //////////////////////////////////////////////////////////////////////////////

  virtual bool isEmbeddable() const = 0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief register the transaction in the context
  //////////////////////////////////////////////////////////////////////////////

  virtual int registerTransaction(struct TRI_transaction_t*) = 0;
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief unregister the transaction
  //////////////////////////////////////////////////////////////////////////////

  virtual void unregisterTransaction() = 0;

 protected:
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief create a resolver
  //////////////////////////////////////////////////////////////////////////////
    
  void createResolver();
 
 protected:
  
  TRI_vocbase_t* _vocbase; 
  
  CollectionNameResolver const* _resolver;
  
  std::shared_ptr<velocypack::CustomTypeHandler> _customTypeHandler;
  
  std::unordered_map<TRI_voc_cid_t, DocumentDitch*> _ditches;

  arangodb::velocypack::Options _options;

  struct {
    TRI_voc_tid_t id; 
    bool hasFailedOperations;
  } _transaction;

  bool _ownsResolver;
};
}

#endif
