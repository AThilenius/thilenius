# This code is really, really hard to explain in comments, let's meet in person
# so I can explain what it does. It was really damn hard to write as well!

def select(search_list, seq_name):
  """
  A helper for the UPGMA algorithm that searches the list of tuples
  (search_list) and returns a union of both forward and backward matches as well
  as the modified list (removing items that were matched).
  Forward matches are pretty obvious, backward matches are needed because we
  have an asymmetric list of items (we have AB and not BA). This is also done to
  make life easier.
  Args:
    search_list The list of tuples to search through
    seq_name The name of the sequence to search for in both forward and backward
             order
  Returns:
    A tuple of (matches, modified list)
  """
  forward = [((left, right), val) for ((left, right), val)
             in search_list if left == seq_name]
  backward = [((right, left), val) for ((left, right), val)
              in search_list if right == seq_name]
  new_search_list = [((left, right), val) for ((left, right), val)
                     in search_list if left != seq_name and right != seq_name]
  return (forward + backward, new_search_list)

def upgma(combination_list):
  """
  Runs the 'UPGMA' algorithm on a list of tuples. The tuples themselves change
  with recursion to the left (aka the left most item in the tuple can be of
  arbitrary depth). For example, a single tuple can look like:
  (('(A,B):4', C), 6)
  meaning A and B have already been grouped, and they have a distance of 4. C is
  the last thing to be grouped up with this tuple. It will be turned into a full
  string once reaching max recursive depth.
  Args:
    combination_list A list of the tuples described above
  Returns:
    A single string in Newick Format that represents the computed tree
  """
  if len(combination_list) == 1:
    ((left, right), value) = combination_list[0]
    return '(%s, %s):%g;' % (left, right, value/2.0)
  # Find the min in the list
  ((left, right), value) = min(combination_list, key=lambda (_, value): value)
  # Remove min from the list
  combination_list = [
    item for item in combination_list if item != ((left, right), value)]
  # Select the first item and sequence
  (sel_left, combination_list) = select(combination_list, left)
  (sel_right, combination_list) = select(combination_list, right)
  # Sort the list so we can select pairs of 2
  selected = sorted(sel_left + sel_right, key=lambda ((l, right), v): right)
  # Grab each set of 2
  for i in xrange(0, len(selected), 2):
    ((_, fr), fv) = selected[i]
    ((_, _), sv) = selected[i + 1]
    combination_list.append((('(%s:%g, %s:%g)' %
        (left, value/2.0, right, value/2.0), fr), (fv + sv) / 2.0))
  return upgma(combination_list) 
