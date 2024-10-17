# Flower-Exchange-Project
 This is the Course project done in LSEG C++ workshop Series

<h2>Project Overview</h2>
<p>
    The Flower Exchange is a basic trading system where traders can submit buy or sell orders for various types of flowers. The system processes these orders using an Exchange Application, matching them against existing orders in the order book to facilitate full or partial executions. Each order submission results in an Execution Report that reflects the status of the order.
</p>

<h2>Features</h2>
<ul>
    <li><strong>Order Matching</strong>: The system supports full and partial executions for both buy and sell orders.</li>
    <li><strong>Order Book Management</strong>: Separate order books are maintained for each flower type (Instrument).</li>
    <li><strong>Order Rejection</strong>: Orders may be rejected for invalid inputs such as incorrect quantities, invalid instruments, or prices that do not meet the system's criteria.</li>
    <li><strong>Execution Report</strong>: Each order generates an execution report that includes order status (new, fill, partial fill, rejected) and reasons for rejection, if applicable.</li>
</ul>

<h2>System Components</h2>
<ul>
    <li><strong>Trader Application</strong>: Allows traders to submit orders.</li>
    <li><strong>Exchange Application</strong>: Processes the incoming orders, matches them against the order book, and executes trades.</li>
    <li><strong>Order Book</strong>: Maintains a list of buy and sell orders for each flower type, sorting them based on price and time priority.</li>
    <li><strong>Execution Report</strong>: Generates reports after each order execution, providing status and transaction details.</li>
</ul>

<h2>Project Workflow</h2>
<ol>
    <li>Traders submit buy or sell orders using the Trader Application.</li>
    <li>The Exchange Application validates and processes the orders.</li>
    <li>If a matching order exists in the order book, the system executes the trade (either fully or partially).</li>
    <li>An Execution Report is generated with the result (fill, partial fill, or rejection) and returned to the trader.</li>
    <li>The order book is updated to reflect any remaining quantities after partial executions.</li>
</ol>

<h2>Input & Output</h2>
<h3>Input (orders.csv)</h3>
<p>The system reads from an <code>orders.csv</code> file, which contains the following fields:</p>
<ul>
    <li><strong>Client Order ID</strong> - Unique identifier for each submitted order.</li>
    <li><strong>Instrument</strong> - The type of flower (Rose, Lavender, Lotus, Tulip, Orchid).</li>
    <li><strong>Side</strong> - 1 for Buy, 2 for Sell.</li>
    <li><strong>Price</strong> - Price per unit (must be greater than 0).</li>
    <li><strong>Quantity</strong> - Order size (must be a multiple of 10, between 10 and 1000).</li>
</ul>

<h3>Output (execution_rep.csv)</h3>
<p>The system produces an <code>execution_rep.csv</code> file with the following fields:</p>
<ul>
    <li><strong>Client Order ID</strong> - Refers to the submitted order ID.</li>
    <li><strong>Order ID</strong> - A unique identifier for the order in the exchange system.</li>
    <li><strong>Instrument</strong> - The type of flower traded.</li>
    <li><strong>Side</strong> - Buy or Sell (1 for Buy, 2 for Sell).</li>
    <li><strong>Price</strong> - Execution price per unit.</li>
    <li><strong>Quantity</strong> - The quantity that was executed.</li>
    <li><strong>Status</strong> - Execution result: 0 for New, 1 for Rejected, 2 for Fill, 3 for Partial Fill.</li>
    <li><strong>Reason</strong> - Explains the rejection reason (if any).</li>
    <li><strong>Transaction Time</strong> - Timestamp of the execution in <code>YYYYMMDD-HHMMSS.sss</code> format.</li>
</ul>

<h2>Validation Rules</h2>
<p>Orders will be rejected if they do not meet the following criteria:</p>
<ul>
    <li>All required fields are present.</li>
    <li>The instrument is valid (Rose, Lavender, Lotus, Tulip, Orchid).</li>
    <li>The side is valid (1 for Buy, 2 for Sell).</li>
    <li>The price is greater than 0.</li>
    <li>The quantity is a multiple of 10 and within the range of 10 to 1000.</li>
</ul>

<h2>How to Run</h2>
<ol>
    <li>Clone the repository from GitHub.</li>
    <li>Ensure that your environment supports C++ compilation.</li>
    <li>Run the main program with the input <code>orders.csv</code> file.</li>
    <li>Review the output <code>execution_rep.csv</code> file to analyze the execution reports.</li>
</ol>

<h2>Evaluation Criteria</h2>
<ul>
    <li><strong>Design and Coding Practices</strong>: Clear and efficient code structure.</li>
    <li><strong>Functional Accuracy</strong>: The system must correctly process orders and generate valid execution reports.</li>
    <li><strong>Performance</strong>: The system should handle large inputs efficiently.</li>
</ul>

<h2>Contributors</h2>
<ul>
    <li>[Mihiraja Kuruppu]</li>
    <li>[Chandeepa Janith]</li>
</ul>

</body>
</html>

