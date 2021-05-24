// leetcode 206: reverse list

pub struct Solution {
}

impl Solution {
    pub fn reverse_list(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut l = head;
        let mut r = None;

        while let Some(mut node) = l {
            l = node.next.take();
            println!("{}", node.val);
            node.next = r;
            r = Some(node);
            // println!("{}", node.val); // already moved, cannot borrow
        }
        return r;
    }
}
