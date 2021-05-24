// leetcode 92: reverse list from m to n

// Definition for singly-linked list.
// #[derive(PartialEq, Eq, Clone, Debug)]
// pub struct ListNode {
//   pub val: i32,
//   pub next: Option<Box<ListNode>>
// }
// 
// impl ListNode {
//   #[inline]
//   fn new(val: i32) -> Self {
//     ListNode {
//       next: None,
//       val
//     }
//   }
// }
impl Solution {
    pub fn reverse_between(head: Option<Box<ListNode>>, m: i32, n: i32) -> Option<Box<ListNode>> {

        let mut l = head.as_ref();
        let mut ret = head;///////

        loop {
            if let Some(node) = l {
                if node.val == m {
                    //println!("{}", node.val);
                    break;
                } else {
                    l = node.next.as_ref();
                }
            }
        }

        let mut r = None;

        while let Some(mut node) = l {
            if (node.val == n) {
                break;
            } else {
                l = node.next.take();
                //println!("{}", node.val);
                node.next = r;
                r = Some(node);
            }
        }
        
        return head;
   }
}
