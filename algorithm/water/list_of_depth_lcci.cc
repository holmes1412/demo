// https://leetcode-cn.com/problems/list-of-depth-lcci/
// return a vector with each list node means the tree node in that depth

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    vector<ListNode*> listOfDepth(TreeNode* tree) {
        queue<pair<TreeNode*, int>> q;
        int level = 0;
        vector<ListNode*> res;
        vector<ListNode*> test_res;
        if (!tree)
            return res;

        ListNode *current_list_head;// = new ListNode(tree->val);
        ListNode *current_list_tail = current_list_head;

        q.push(make_pair(tree, 0));

        while (!q.empty())
        {
            pair<TreeNode*, int> pair = q.front();
            q.pop();
            TreeNode *node = pair.first;

            if (node->left)
                q.push(make_pair(node->left, pair.second + 1));
            if (node->right)
                q.push(make_pair(node->right, pair.second + 1));

            if (pair.second > level)
            {
                level++;
                ListNode *tmp = current_list_head;
                //while(tmp)
                //{test_res.push_back(new ListNode(tmp->val)); tmp = tmp->next;}
                res.push_back(current_list_head);
                current_list_head = new ListNode(node->val);
                current_list_tail = current_list_head->next;
            }
            else
            {
                if (level == 0)
                {
                    current_list_head = new ListNode(node->val);
                    current_list_tail = current_list_head->next;
                }
                else
                {
                    //test_res.push_back(new ListNode(node->val));
                    current_list_tail = new ListNode(node->val);
                    current_list_tail = current_list_tail->next;
                }
            }

            res.push_back(current_list_head);
        }
        return res;
    }
};
