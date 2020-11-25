#include <functional>
#include <iostream> //debug only

namespace bst {
  //bst traits helper structures
  struct traverse_order {};

  struct preorder : traverse_order {};
  struct inorder : traverse_order {};
  struct postorder : traverse_order {};

  //binary search tree template class
  template <typename T>
  class Tree {
  public:
    using value_t       = T;
    using traverse_fn_t = std::function<void(const value_t&)>;

  private:
    struct node {
      node *left, *right;
      value_t val;

      node(const value_t& v)
        : val(v)
        , left(nullptr)
        , right(nullptr)
      { }
    };

  public:
    Tree() noexcept
      : _root(nullptr)
    { }

    Tree(value_t&& val) noexcept
      : _root(new node(std::move(val)))
    { }

    Tree(std::initializer_list<value_t> list) noexcept
      : _root(nullptr)
    {
      insert(list);
    }

    //disallow copy
    Tree(const Tree&) noexcept = delete;
    Tree& operator=(const Tree&) noexcept = delete;

    //allow move
    Tree(Tree&&) noexcept = default;
    Tree& operator=(Tree&&) noexcept = default;

    ~Tree() {
      clear();
    }

    node* insert(const value_t& val) {
      return insert(&_root, val);
    }

    node* insert(std::initializer_list<value_t> list) {
      node* last = nullptr;
      for(const auto& item : list) {
        last = insert(item);
      }
      return last;
    }

#if 0
    void erase(const value_t& val) {
      erase(&_root, val);
    }
#endif //broken

  public:
    template<typename Order = preorder>
    auto traverse(
      const traverse_fn_t& predicate,
      typename std::enable_if<std::is_base_of<traverse_order, Order>::value, void>::type* dummy = nullptr) -> void
    {
      if(_root)
        traverse<Order>(_root, predicate);
    }

    void clear() {
      if(_root)
        clear(_root);
    }

  private:
    template<typename Order = preorder>
    auto traverse(
      node* const root,
      const traverse_fn_t& predicate,
      typename std::enable_if<std::is_base_of<traverse_order, Order>::value, void>::type* dummy = nullptr) -> void
    {
      //std++ >=17
      if constexpr (std::is_same<Order,preorder>::value) {
        predicate(root->val);
        if(root->left) traverse<Order>(root->left, predicate);
        if(root->right) traverse<Order>(root->right, predicate);
      }
      if constexpr (std::is_same<Order,inorder>::value) {
        if(root->left) traverse<Order>(root->left, predicate);
        predicate(root->val);
        if(root->right) traverse<Order>(root->right, predicate);
      }
      if constexpr (std::is_same<Order,postorder>::value) {
        if(root->left) traverse<Order>(root->left, predicate);
        if(root->right) traverse<Order>(root->right, predicate);
        predicate(root->val);
      }
    }

    node* insert(node** root, const value_t& val) {
      if(*root) {
        if(val < (*root)->val) {
          return insert(&((*root)->left), val);
        } else {
          return insert(&((*root)->right), val);
        }
      } else {
        return *root = new node(val);
      }
    }

#if 0
    void erase(node** root, const value_t& val) {
      if(*root) {
        if(val == (*root)->val) {
          std::cout << "removed: " << val << std::endl;
          delete (*root);
        } else {
          if(val < (*root)->val) {
            erase(&((*root)->left), val);
          } else {
            erase(&((*root)->right), val);
          }
        }
      }
    }
#endif //broken

    void clear(node* const root) {
      node* left = root->left;
      node* right = root->right;
#if DEBUG
      std::cout << "deleted: " << root->val << std::endl;
#endif //DEBUG
      delete root;
      if(left) clear(left);
      if(right) clear(right);
    }

  private:
    node *_root;

  };
}

int main() {
  using namespace bst;

  Tree b_tree {2, 1, 3};

  auto print = [](int n) {
    std::cout << n << std::endl;
  };

  b_tree.traverse<preorder>( print );
  b_tree.traverse<inorder>( print );
  b_tree.traverse<postorder>( print );

  return 0;
}
