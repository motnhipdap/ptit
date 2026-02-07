import React, { useEffect, useState } from 'react';
import axios from 'axios';
import { backendUrl } from '../App';
import { toast } from 'react-toastify';

const Tickets = ({ token }) => {
  const [tickets, setTickets] = useState([]);
  const [loading, setLoading] = useState(true);
  const [selectedTicket, setSelectedTicket] = useState(null);
  const [replyMessage, setReplyMessage] = useState('');
  const [isSubmitting, setIsSubmitting] = useState(false);

  // Fetch all tickets
  const fetchTickets = async () => {
    try {
      setLoading(true);
      const response = await axios.get(backendUrl + '/api/tickets/', {
        headers: { token }
      });
      
      // Backend returns tickets directly as array
      if (Array.isArray(response.data)) {
        setTickets(response.data);
      }
    } catch (error) {
      console.error(error);
      toast.error(error.response?.data?.message || 'Failed to fetch tickets');
    } finally {
      setLoading(false);
    }
  };

  // Fetch ticket details
  const fetchTicketDetails = async (ticketId) => {
    try {
      const response = await axios.get(backendUrl + `/api/tickets/${ticketId}`, {
        headers: { token }
      });
      
      // Backend returns ticket directly as object
      if (response.data) {
        setSelectedTicket(response.data);
      }
    } catch (error) {
      console.error(error);
      toast.error('Failed to fetch ticket details');
    }
  };

  // Reply to ticket
  const handleReply = async (e) => {
    e.preventDefault();
    
    if (!replyMessage.trim()) {
      toast.error('Please enter a reply message');
      return;
    }

    setIsSubmitting(true);
    try {
      const response = await axios.post(
        backendUrl + `/api/tickets/${selectedTicket._id}/reply`,
        { message: replyMessage },
        { headers: { token } }
      );

      if (response.data.message) {
        toast.success('Reply sent successfully');
        setReplyMessage('');
        // Refresh ticket details
        fetchTicketDetails(selectedTicket._id);
        // Refresh tickets list
        fetchTickets();
      }
    } catch (error) {
      console.error(error);
      toast.error(error.response?.data?.message || 'Failed to send reply');
    } finally {
      setIsSubmitting(false);
    }
  };

  // Close ticket
  const handleCloseTicket = async () => {
    if (!window.confirm('Are you sure you want to close this ticket?')) {
      return;
    }

    try {
      const response = await axios.patch(
        backendUrl + `/api/tickets/${selectedTicket._id}/close`,
        {},
        { headers: { token } }
      );

      if (response.data.message) {
        toast.success('Ticket closed successfully');
        // Refresh ticket details
        fetchTicketDetails(selectedTicket._id);
        // Refresh tickets list
        fetchTickets();
      }
    } catch (error) {
      console.error(error);
      toast.error(error.response?.data?.message || 'Failed to close ticket');
    }
  };

  useEffect(() => {
    fetchTickets();
  }, [token]);

  const formatDate = (dateString) => {
    const date = new Date(dateString);
    return date.toLocaleDateString('en-US', { 
      year: 'numeric', 
      month: 'short', 
      day: 'numeric',
      hour: '2-digit',
      minute: '2-digit'
    });
  };

  if (loading) {
    return (
      <div className='flex justify-center items-center h-64'>
        <div className='text-xl text-gray-600'>Loading tickets...</div>
      </div>
    );
  }

  return (
    <div className='max-w-7xl mx-auto'>
      <h1 className='text-3xl font-bold mb-8 text-gray-800'>Support Tickets</h1>

      <div className='flex gap-6'>
        {/* Tickets List */}
        <div className='w-2/5 bg-white rounded-lg shadow-md p-6 max-h-[calc(100vh-200px)] overflow-y-auto'>
          <h2 className='text-xl font-semibold mb-4 text-gray-700'>All Tickets</h2>
          
          {tickets.length === 0 ? (
            <p className='text-gray-500 text-center py-8'>No tickets found</p>
          ) : (
            <div className='space-y-3'>
              {tickets.map((ticket) => (
                <div
                  key={ticket._id}
                  onClick={() => fetchTicketDetails(ticket._id)}
                  className={`p-4 border rounded-lg cursor-pointer transition-all hover:shadow-md ${
                    selectedTicket?._id === ticket._id
                      ? 'border-blue-500 bg-blue-50'
                      : 'border-gray-200 hover:border-gray-300'
                  }`}
                >
                  <div className='flex justify-between items-start mb-2'>
                    <h3 className='font-semibold text-gray-800'>{ticket.productName}</h3>
                    <span
                      className={`px-2 py-1 text-xs rounded-full ${
                        ticket.status === 'open'
                          ? 'bg-green-100 text-green-800'
                          : 'bg-gray-100 text-gray-800'
                      }`}
                    >
                      {ticket.status}
                    </span>
                  </div>
                  <p className='text-sm text-gray-600 mb-1'>Order ID: {ticket.orderId}</p>
                  <p className='text-xs text-gray-500'>
                    {ticket.message.length} message{ticket.message.length !== 1 ? 's' : ''}
                  </p>
                </div>
              ))}
            </div>
          )}
        </div>

        {/* Ticket Details */}
        <div className='w-3/5 bg-white rounded-lg shadow-md p-6'>
          {!selectedTicket ? (
            <div className='flex flex-col items-center justify-center h-full text-gray-500'>
              <svg className='w-24 h-24 mb-4' fill='none' stroke='currentColor' viewBox='0 0 24 24'>
                <path strokeLinecap='round' strokeLinejoin='round' strokeWidth={1.5} d='M8 10h.01M12 10h.01M16 10h.01M9 16H5a2 2 0 01-2-2V6a2 2 0 012-2h14a2 2 0 012 2v8a2 2 0 01-2 2h-5l-5 5v-5z' />
              </svg>
              <p className='text-lg'>Select a ticket to view details</p>
            </div>
          ) : (
            <div className='h-full flex flex-col'>
              {/* Ticket Header */}
              <div className='border-b pb-4 mb-4'>
                <div className='flex justify-between items-start mb-3'>
                  <div>
                    <h2 className='text-2xl font-bold text-gray-800'>{selectedTicket.productName}</h2>
                    <p className='text-gray-600'>Order ID: {selectedTicket.orderId}</p>
                  </div>
                  <div className='flex items-center gap-2'>
                    <span
                      className={`px-3 py-1 text-sm rounded-full font-medium ${
                        selectedTicket.status === 'open'
                          ? 'bg-green-100 text-green-800'
                          : 'bg-gray-100 text-gray-800'
                      }`}
                    >
                      {selectedTicket.status}
                    </span>
                    {selectedTicket.status === 'open' && (
                      <button
                        onClick={handleCloseTicket}
                        className='px-3 py-1 text-sm bg-red-500 text-white rounded-full hover:bg-red-600 transition-colors font-medium'
                      >
                        Close Ticket
                      </button>
                    )}
                  </div>
                </div>
                
                {/* Product Image */}
                {selectedTicket.image && (
                  <div className='mt-3'>
                    <img
                      src={backendUrl + '/' + selectedTicket.image}
                      alt='Product issue'
                      className='max-h-64 rounded-lg border border-gray-200'
                      onError={(e) => {
                        e.target.onerror = null;
                        e.target.src = 'data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjAwIiBoZWlnaHQ9IjIwMCIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48cmVjdCB3aWR0aD0iMjAwIiBoZWlnaHQ9IjIwMCIgZmlsbD0iI2VlZSIvPjx0ZXh0IHg9IjUwJSIgeT0iNTAlIiBmb250LXNpemU9IjE4IiBmaWxsPSIjOTk5IiB0ZXh0LWFuY2hvcj0ibWlkZGxlIiBkeT0iLjNlbSI+SW1hZ2UgTm90IEZvdW5kPC90ZXh0Pjwvc3ZnPg==';
                      }}
                    />
                  </div>
                )}
              </div>

              {/* Messages */}
              <div className='flex-1 overflow-y-auto mb-4 space-y-4 max-h-[400px]'>
                {selectedTicket.message.map((msg, index) => (
                  <div
                    key={index}
                    className={`flex ${msg.sender === 'admin' ? 'justify-end' : 'justify-start'}`}
                  >
                    <div
                      className={`max-w-[75%] p-4 rounded-lg ${
                        msg.sender === 'admin'
                          ? 'bg-blue-500 text-white'
                          : 'bg-gray-100 text-gray-800'
                      }`}
                    >
                      <div className='flex items-center gap-2 mb-1'>
                        <span className='font-semibold text-sm'>
                          {msg.sender === 'admin' ? 'Admin' : 'Customer'}
                        </span>
                        <span className={`text-xs ${msg.sender === 'admin' ? 'text-blue-100' : 'text-gray-500'}`}>
                          {formatDate(msg.createdAt)}
                        </span>
                      </div>
                      <p className='whitespace-pre-wrap'>{msg.text}</p>
                    </div>
                  </div>
                ))}
              </div>

              {/* Reply Form */}
              {selectedTicket.status === 'open' && (
                <form onSubmit={handleReply} className='border-t pt-4'>
                  <div className='mb-3'>
                    <textarea
                      value={replyMessage}
                      onChange={(e) => setReplyMessage(e.target.value)}
                      placeholder='Type your reply...'
                      rows='3'
                      className='w-full px-4 py-3 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500 resize-none'
                    />
                  </div>
                  <button
                    type='submit'
                    disabled={isSubmitting || !replyMessage.trim()}
                    className='w-full bg-blue-600 text-white py-3 rounded-md hover:bg-blue-700 transition-colors disabled:bg-gray-400 disabled:cursor-not-allowed font-medium'
                  >
                    {isSubmitting ? 'Sending...' : 'Send Reply'}
                  </button>
                </form>
              )}
              {selectedTicket.status === 'closed' && (
                <div className='border-t pt-4 text-center text-gray-500'>
                  <p className='text-sm'>This ticket is closed. No further replies can be sent.</p>
                </div>
              )}
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

export default Tickets;
