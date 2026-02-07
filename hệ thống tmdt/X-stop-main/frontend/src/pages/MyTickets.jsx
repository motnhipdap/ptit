import React, { useContext, useEffect, useState } from 'react';
import { ShopContext } from '../context/ShopContext';
import Title from '../components/Title';
import axios from 'axios';
import { toast } from 'react-toastify';

const MyTickets = () => {
  const { backendUrl, token } = useContext(ShopContext);
  const [tickets, setTickets] = useState([]);
  const [selectedTicket, setSelectedTicket] = useState(null);
  const [loading, setLoading] = useState(true);

  // Fetch user's tickets
  const fetchMyTickets = async () => {
    try {
      if (!token) {
        return;
      }
      setLoading(true);
      const response = await axios.get(backendUrl + '/api/tickets/', {
        headers: { token }
      });

      if (Array.isArray(response.data)) {
        setTickets(response.data);
      }
    } catch (error) {
      console.error(error);
      toast.error('Failed to load tickets');
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

      if (response.data) {
        setSelectedTicket(response.data);
      }
    } catch (error) {
      console.error(error);
      toast.error('Failed to load ticket details');
    }
  };

  useEffect(() => {
    fetchMyTickets();
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

  if (!token) {
    return (
      <div className='border-t pt-16'>
        <div className='text-center'>
          <Title text1={'MY'} text2={'TICKETS'} />
          <p className='text-gray-500 mt-8'>Please login to view your support tickets</p>
        </div>
      </div>
    );
  }

  if (loading) {
    return (
      <div className='border-t pt-16'>
        <div className='text-center'>
          <Title text1={'MY'} text2={'TICKETS'} />
          <p className='text-gray-500 mt-8'>Loading your tickets...</p>
        </div>
      </div>
    );
  }

  return (
    <div className='border-t pt-16'>
      <div className='text-2xl mb-8'>
        <Title text1={'MY'} text2={'SUPPORT TICKETS'} />
      </div>

      {tickets.length === 0 ? (
        <div className='text-center py-12'>
          <p className='text-gray-500 text-lg'>You haven't raised any support tickets yet</p>
        </div>
      ) : (
        <div className='flex flex-col lg:flex-row gap-6'>
          {/* Tickets List */}
          <div className='lg:w-1/3 space-y-4'>
            <h3 className='text-lg font-semibold text-gray-800 mb-4'>Your Tickets</h3>
            {tickets.map((ticket) => (
              <div
                key={ticket._id}
                onClick={() => fetchTicketDetails(ticket._id)}
                className={`border rounded-lg p-4 cursor-pointer transition-all hover:shadow-md ${
                  selectedTicket?._id === ticket._id
                    ? 'border-black bg-gray-50'
                    : 'border-gray-300 hover:border-gray-400'
                }`}
              >
                <div className='flex justify-between items-start mb-2'>
                  <h4 className='font-semibold text-gray-800'>{ticket.productName}</h4>
                  <span
                    className={`px-2 py-1 text-xs rounded-full ${
                      ticket.status === 'open'
                        ? 'bg-green-100 text-green-800'
                        : 'bg-gray-200 text-gray-700'
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

          {/* Ticket Details */}
          <div className='lg:w-2/3'>
            {!selectedTicket ? (
              <div className='border rounded-lg p-12 text-center text-gray-500'>
                <svg
                  className='w-20 h-20 mx-auto mb-4 text-gray-400'
                  fill='none'
                  stroke='currentColor'
                  viewBox='0 0 24 24'
                >
                  <path
                    strokeLinecap='round'
                    strokeLinejoin='round'
                    strokeWidth={1.5}
                    d='M8 10h.01M12 10h.01M16 10h.01M9 16H5a2 2 0 01-2-2V6a2 2 0 012-2h14a2 2 0 012 2v8a2 2 0 01-2 2h-5l-5 5v-5z'
                  />
                </svg>
                <p className='text-lg'>Select a ticket to view conversation</p>
              </div>
            ) : (
              <div className='border rounded-lg p-6'>
                {/* Ticket Header */}
                <div className='border-b pb-4 mb-6'>
                  <div className='flex justify-between items-start mb-3'>
                    <div>
                      <h2 className='text-2xl font-bold text-gray-800'>
                        {selectedTicket.productName}
                      </h2>
                      <p className='text-gray-600'>Order ID: {selectedTicket.orderId}</p>
                    </div>
                    <span
                      className={`px-3 py-1 text-sm rounded-full font-medium ${
                        selectedTicket.status === 'open'
                          ? 'bg-green-100 text-green-800'
                          : 'bg-gray-200 text-gray-700'
                      }`}
                    >
                      {selectedTicket.status}
                    </span>
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
                          e.target.style.display = 'none';
                        }}
                      />
                    </div>
                  )}
                </div>

                {/* Conversation */}
                <div className='space-y-4'>
                  <h3 className='text-lg font-semibold text-gray-800 mb-4'>Conversation</h3>
                  {selectedTicket.message.map((msg, index) => (
                    <div
                      key={index}
                      className={`flex ${msg.sender === 'admin' ? 'justify-start' : 'justify-end'}`}
                    >
                      <div
                        className={`max-w-[75%] p-4 rounded-lg ${
                          msg.sender === 'admin'
                            ? 'bg-gray-100 text-gray-800'
                            : 'bg-black text-white'
                        }`}
                      >
                        <div className='flex items-center gap-2 mb-1'>
                          <span className='font-semibold text-sm'>
                            {msg.sender === 'admin' ? '🛡️ Support Team' : '👤 You'}
                          </span>
                          <span
                            className={`text-xs ${
                              msg.sender === 'admin' ? 'text-gray-500' : 'text-gray-300'
                            }`}
                          >
                            {formatDate(msg.createdAt)}
                          </span>
                        </div>
                        <p className='whitespace-pre-wrap text-sm'>{msg.text}</p>
                      </div>
                    </div>
                  ))}
                </div>

                {/* Status Message */}
                <div className='mt-6 pt-4 border-t'>
                  {selectedTicket.status === 'open' ? (
                    <p className='text-sm text-gray-600 text-center'>
                      ✅ This ticket is open. Our support team will respond soon.
                    </p>
                  ) : (
                    <p className='text-sm text-gray-600 text-center'>
                      🔒 This ticket has been closed. If you need further assistance, please raise a new
                      ticket.
                    </p>
                  )}
                </div>
              </div>
            )}
          </div>
        </div>
      )}
    </div>
  );
};

export default MyTickets;
