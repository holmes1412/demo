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
    bool hasCycle(ListNode *head) {
        ListNode *odd;
        ListNode *even;
        if (!head || !head->next)
            return false;
        odd = head;
        even = head->next;
        while (odd)
        {
            if (even == NULL)
                return false;
            if (odd == even)
                return true;
            odd = odd->next;
            if (!even->next || !even->next->next)
                return false;
            even = even->next->next;
        }
        return true;
    }
};
