import React, { useState } from 'react';
import './Dashboard.css'; // Assuming you have a CSS file for styles

const Dashboard = () => {
  // State for Event Details
  const [eventName, setEventName] = useState('');
  const [eventDate, setEventDate] = useState('');
  const [eventTime, setEventTime] = useState('');
  const [eventLocation, setEventLocation] = useState('');

  // State for Guest List
  const [totalGuests, setTotalGuests] = useState('');
  const [invitationsSent, setInvitationsSent] = useState('');
  const [rsvpsReceived, setRsvpsReceived] = useState('');

  // State for To-Do List
  const [beforeEventTasks, setBeforeEventTasks] = useState(Array(8).fill(false));
  const [duringEventTasks, setDuringEventTasks] = useState(Array(8).fill(false));
  const [afterEventTasks, setAfterEventTasks] = useState(Array(8).fill(false));

  // State for Budget
  const [totalBudget, setTotalBudget] = useState('');
  const [budgetBreakdown1, setBudgetBreakdown1] = useState('');
  const [budgetBreakdown2, setBudgetBreakdown2] = useState('');
  const [budgetBreakdown3, setBudgetBreakdown3] = useState('');

  // Sample task labels (you can customize these)
  const beforeTasks = [
    'Book venue', 'Send invitations', 'Plan menu', 'Arrange decorations',
    'Hire entertainment', 'Confirm vendors', 'Prepare guest list', 'Set up transportation'
  ];
  const duringTasks = [
    'Welcome guests', 'Manage check-in', 'Oversee catering', 'Coordinate activities',
    'Handle emergencies', 'Take photos', 'Monitor schedule', 'Ensure safety'
  ];
  const afterTasks = [
    'Clean up venue', 'Send thank-yous', 'Review feedback', 'Pay vendors',
    'Store decorations', 'Analyze budget', 'Plan follow-up', 'Archive photos'
  ];

  const handleCheckboxChange = (section, index) => {
    if (section === 'before') {
      const newTasks = [...beforeEventTasks];
      newTasks[index] = !newTasks[index];
      setBeforeEventTasks(newTasks);
    } else if (section === 'during') {
      const newTasks = [...duringEventTasks];
      newTasks[index] = !newTasks[index];
      setDuringEventTasks(newTasks);
    } else if (section === 'after') {
      const newTasks = [...afterEventTasks];
      newTasks[index] = !newTasks[index];
      setAfterEventTasks(newTasks);
    }
  };

  return (
    <div className="dashboard">
      <div className="dashboard-grid">
        {/* Left Column */}
        <div className="left-column">
          {/* Event Details Card */}
          <div className="card event-details">
            <div className="card-header yellow">Event Details</div>
            <div className="card-content">
              <div className="field-row">
                <label>Name:</label>
                <input
                  type="text"
                  value={eventName}
                  onChange={(e) => setEventName(e.target.value)}
                  className="underline-input"
                />
              </div>
              <div className="field-row">
                <label>Date:</label>
                <input
                  type="text"
                  value={eventDate}
                  onChange={(e) => setEventDate(e.target.value)}
                  className="underline-input"
                />
                <label>Time:</label>
                <input
                  type="text"
                  value={eventTime}
                  onChange={(e) => setEventTime(e.target.value)}
                  className="underline-input"
                />
              </div>
              <div className="field-row">
                <label>Location:</label>
                <input
                  type="text"
                  value={eventLocation}
                  onChange={(e) => setEventLocation(e.target.value)}
                  className="underline-input"
                />
              </div>
            </div>
          </div>

          {/* To-Do List Card */}
          <div className="card todo-list">
            <div className="card-header green">To-Do List</div>
            <div className="card-content">
              <div className="subsection">
                <h4>Before Event</h4>
                {beforeTasks.map((task, index) => (
                  <div key={index} className="checkbox-row">
                    <input
                      type="checkbox"
                      checked={beforeEventTasks[index]}
                      onChange={() => handleCheckboxChange('before', index)}
                    />
                    <span className="checkbox-line">{task}</span>
                  </div>
                ))}
              </div>
              <div className="subsection">
                <h4>During Event</h4>
                {duringTasks.map((task, index) => (
                  <div key={index} className="checkbox-row">
                    <input
                      type="checkbox"
                      checked={duringEventTasks[index]}
                      onChange={() => handleCheckboxChange('during', index)}
                    />
                    <span className="checkbox-line">{task}</span>
                  </div>
                ))}
              </div>
              <div className="subsection">
                <h4>After Event</h4>
                {afterTasks.map((task, index) => (
                  <div key={index} className="checkbox-row">
                    <input
                      type="checkbox"
                      checked={afterEventTasks[index]}
                      onChange={() => handleCheckboxChange('after', index)}
                    />
                    <span className="checkbox-line">{task}</span>
                  </div>
                ))}
              </div>
            </div>
          </div>
        </div>

        {/* Right Column */}
        <div className="right-column">
          {/* Guest List Card */}
          <div className="card guest-list">
            <div className="card-header teal">Guest List</div>
            <div className="card-content">
              <div className="field-row">
                <label>Total Guests:</label>
                <input
                  type="text"
                  value={totalGuests}
                  onChange={(e) => setTotalGuests(e.target.value)}
                  className="underline-input"
                />
              </div>
              <div className="field-row">
                <label>Invitations Sent:</label>
                <input
                  type="text"
                  value={invitationsSent}
                  onChange={(e) => setInvitationsSent(e.target.value)}
                  className="underline-input"
                />
              </div>
              <div className="field-row">
                <label>RSVPs Received:</label>
                <input
                  type="text"
                  value={rsvpsReceived}
                  onChange={(e) => setRsvpsReceived(e.target.value)}
                  className="underline-input"
                />
              </div>
            </div>
          </div>

          {/* Budget Card */}
          <div className="card budget">
            <div className="card-header purple">Budget</div>
            <div className="card-content">
              <div className="field-row">
                <label>Total Budget:</label>
                <input
                  type="text"
                  value={totalBudget}
                  onChange={(e) => setTotalBudget(e.target.value)}
                  className="underline-input"
                />
              </div>
              <div className="field-row">
                <label>Breakdown 1:</label>
                <input
                  type="text"
                  value={budgetBreakdown1}
                  onChange={(e) => setBudgetBreakdown1(e.target.value)}
                  className="underline-input"
                />
              </div>
              <div className="field-row">
                <label>Breakdown 2:</label>
                <input
                  type="text"
                  value={budgetBreakdown2}
                  onChange={(e) => setBudgetBreakdown2(e.target.value)}
                  className="underline-input"
                />
              </div>
              <div className="field-row">
                <label>Breakdown 3:</label>
                <input
                  type="text"
                  value={budgetBreakdown3}
                  onChange={(e) => setBudgetBreakdown3(e.target.value)}
                  className="underline-input"
                />
              </div>
            </div>
          </div>

          {/* Agenda Card */}
          <div className="card agenda">
            <div className="card-header orange">Agenda</div>
            <div className="card-content">
              {/* Placeholder for agenda items - you can add more fields or lists here */}
              <p>Agenda items go here...</p>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};

export default Dashboard;